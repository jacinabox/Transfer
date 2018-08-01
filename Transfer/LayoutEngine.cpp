#include "stdafx.h"
#include <vector>
#include <iterator>
#include <math.h>
//#include <algorithm>

/////////////////////////////////////////

struct INTERNAL_STATE {
	POINT left_point, right_point, current_point;
};

struct LINE_WIDTH_RESULT {
	unsigned width;
	unsigned n_objects;
	bool fits_on_one_line_flag;
};

LINE_WIDTH_RESULT line_width(const INTERNAL_STATE& is, LO_ITERATOR begin, LO_ITERATOR end) {
	POINT point(is.left_point);
	LINE_WIDTH_RESULT lwr = {0};
	LO_ITERATOR it;
	
	for (it=begin;it!=end;++it) {
		if ((*it)->get_float_type()==INLINE) {
			if (point.x + (*it)->get_width() > is.right_point.x) return lwr;

			point.x += (*it)->get_width();
			lwr.width += (*it)->get_width();
			lwr.n_objects++;
		}
	}

	lwr.fits_on_one_line_flag =true;
	return lwr;
}

void layout_line(const LINE_WIDTH_RESULT& lwr, const INTERNAL_STATE& is, LO_ITERATOR begin, LO_ITERATOR end, LayoutDelegate* ld,
	std::vector<LAYOUT_LINE_RESULT>& result) {
	LAYOUT_LINE_RESULT temp_llr;
	LO_ITERATOR it;

	//Tell the layout delegate to start a new line.
	ld->start_new_line(is.left_point.x, is.right_point.x, lwr.width);

	for (it=begin;it!=end;++it) {

		if ((*it)->get_float_type()==INLINE) {

			//The layout width includes any padding to be dynamically added; it also includes horizontal spacing
			//between words.
			temp_llr.point.x = ld->layout_line(**it);
			temp_llr.point.y = is.current_point.y;
		} else {
			//TODO testing with prototype revealed that floated elements should have a presence also as inline 
			// elements, so can tell where they are anchored in the text.
			//Computing positions of floated elements is done in 'layoutFloatedLeft'/'layoutFloatedRight'.	
			temp_llr.point = {0,0};
		}

		temp_llr.lo = *it;

		result.push_back(temp_llr);
	}

}

void layout_floated_left(INTERNAL_STATE& is,
	std::vector<LAYOUT_LINE_RESULT>::iterator begin,
	std::vector<LAYOUT_LINE_RESULT>::iterator end) {

	std::vector<LAYOUT_LINE_RESULT>::iterator it;

	for (it = begin;it != end;++it) {
		it->point.x = 0;
		it->point.y = is.left_point.y;

		if (it->lo->get_width() > is.left_point.x) {
			is.left_point.x = it->lo->get_width();
		}
		is.left_point.y += it->lo->get_height();
	}
}















void layout(int viewport_width, const std::vector<Paragraph>& vector, std::vector<LAYOUT_LINE_RESULT>& result) {
	INTERNAL_STATE is = { {0, 0}, {viewport_width, 0}, {0, 0} };
	std::vector<Paragraph>::const_iterator it = vector.cbegin();
	LO_ITERATOR it2;
	std::vector<LAYOUT_LINE_RESULT>::iterator it3;
	std::vector<LAYOUT_LINE_RESULT> line_result, line_result_inlines, line_result_left_floats,
		line_result_right_floats;
	LAYOUT_LINE_RESULT temp;
	LINE_WIDTH_RESULT lwr;

	result.clear();

	if (it == vector.cend()) return;

	it2 = it->vector.cbegin();

	//Each iteration of the loop processes a single line (which has little to do with the structure
	//of paragraphs, and is dictated by how many objects fit in the space).
	while (it != vector.cend()) {
		if (it2 == it->vector.cend()) {
			++it;
			if (it==vector.cend()) break;
			it2 = it->vector.cbegin();
			continue;
		}

		//Calculate how many objects fit on the current line.
		lwr = line_width(is, it2, it->vector.cend());

		layout_line(lwr, is, it2, it->vector.cend(), it->ld, line_result);
		it2 += lwr.n_objects;

		//Next partition the results by their float type (layout type). Each float type is handled specially.
		for (it3=line_result.begin();it3!=line_result.end();++it3) {
			switch (it3->lo->get_float_type()) {
			case INLINE:
				line_result_inlines.push_back(*it3);
				break;
			case FLOAT_LEFT:
				line_result_left_floats.push_back(*it3);
				break;
			case FLOAT_RIGHT:
				line_result_right_floats.push_back(*it3);
				break;
			}

		}

		line_result_inlines.resize(lwr.n_objects);

		//If there isn't enough space, can still position one element below all other elements.
		if (line_result_inlines.empty()
			&& it2 != it->vector.cend()
			&& (*it2)->get_float_type()==INLINE) {

			is.current_point.x = 0;
			is.current_point.y = max(is.left_point.y, max(is.right_point.y, is.current_point.y));

			temp = { { 0, is.current_point.y }, *(it2++) };
			line_result_inlines.push_back(temp);
		}

		result.insert(result.end(), line_result_inlines.begin(), line_result_inlines.end());
		
		//Update the current y-coordinate.
		unsigned temp_y;
		for (it3 = line_result_inlines.begin();it3 != line_result_inlines.end();++it3) {
			temp_y = it3->point.y + it3->lo->get_height();
			if (temp_y > is.current_point.y) is.current_point.y = temp_y;
		}
		if (is.current_point.y > is.left_point.y) is.left_point.y = is.current_point.y;
		if (is.current_point.y > is.right_point.y) is.right_point.y = is.current_point.y;

		layout_floated_left(is, line_result_left_floats.begin(), line_result_left_floats.end());
		it2 += line_result_left_floats.size();

		result.insert(result.end(), line_result_left_floats.begin(), line_result_left_floats.end());

		//Reset "x" coordinates:
		if (is.left_point.y <= is.current_point.y) {
			is.left_point.x = 0;
		}
		if (is.right_point.y <= is.current_point.y) {
			is.right_point.x = viewport_width;
		}

		
		line_result.clear();
		line_result_inlines.clear();
		line_result_left_floats.clear();
		line_result_right_floats.clear();
	}
}

///////////////////////////////////////

LeftJustifyingLayoutDelegate::LeftJustifyingLayoutDelegate() {
}
LeftJustifyingLayoutDelegate ::~LeftJustifyingLayoutDelegate() {
}
void LeftJustifyingLayoutDelegate::start_new_line(unsigned left_extent, unsigned right_extent, unsigned line_width) {
	x = left_extent;
}
unsigned LeftJustifyingLayoutDelegate::layout_line(const LayoutObject& lo) {
	unsigned width = lo.get_width();
	unsigned x2 = x;

	x += width;

	return x2;
}

LayoutDelegate* left_justifier() {
	return new LeftJustifyingLayoutDelegate();
}

///////////////////////////////////////

RightJustifyingLayoutDelegate::RightJustifyingLayoutDelegate() {
}

RightJustifyingLayoutDelegate::~RightJustifyingLayoutDelegate() {
}

void RightJustifyingLayoutDelegate::start_new_line(unsigned left_extent, unsigned right_extent, unsigned line_width) {
	x = right_extent - line_width;
}
unsigned RightJustifyingLayoutDelegate::layout_line(const LayoutObject& lo) {
	unsigned width = lo.get_width();
	unsigned x2 = x;

	x += width;

	return x2;
}

LayoutDelegate* right_justifier() {
	return new RightJustifyingLayoutDelegate();
}

/////////////////////////////////////


CenterJustifyingLayoutDelegate::CenterJustifyingLayoutDelegate() {
}

CenterJustifyingLayoutDelegate::~CenterJustifyingLayoutDelegate() {
}

void CenterJustifyingLayoutDelegate::start_new_line(unsigned left_extent, unsigned right_extent, unsigned line_width) {
	x = left_extent + (right_extent - line_width) / 2;
}
unsigned CenterJustifyingLayoutDelegate::layout_line(const LayoutObject& lo) {
	unsigned width = lo.get_width();
	unsigned x2 = x;

	x += width;

	return x2;
}

LayoutDelegate* center_justifier() {
	return new CenterJustifyingLayoutDelegate();
}