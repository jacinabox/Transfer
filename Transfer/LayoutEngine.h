#ifndef _LAYOUT_ENGINE
#define _LAYOUT_ENGINE

//A stripped down, minimalistic, layout engine.

#include <vector>

#ifdef WIN32
#include <Windows.h>
#else
struct POINT {
	int x, y;
};
#endif

enum FLOAT_TYPE { INLINE, FLOAT_LEFT, FLOAT_RIGHT };

//A layout object is the fundamental base class of all objects that will be laid out
//by this layout engine.
class LayoutObject {
public:
	LayoutObject() {
	}
	virtual ~LayoutObject() {
	}
	virtual unsigned get_width() const = 0;
	virtual unsigned get_height() const = 0;
	virtual FLOAT_TYPE get_float_type() const = 0;
};

//The job of a layout delegate is to take a vector of layout objects, plus additional
//data, and decide how to horizontally dimension those objects. A layout delegate
//is only asked to dimension inline objects, never floated objects.
class LayoutDelegate {
public:
	LayoutDelegate() {
	}
	virtual ~LayoutDelegate() {
	}
	//line_width is the number of pixels horizontally that are occupied by objects; i.e. the sum of the
	//widths of the objects that are on the line.
	virtual void start_new_line(unsigned left_extent, unsigned right_extent, unsigned line_width) = 0;
	virtual unsigned layout_line(const LayoutObject& lo) = 0;
};

//A paragraph associates a vector of layout objects with a layout delegate specifying how to arrange them.
struct Paragraph {
	std::vector<const LayoutObject*> vector;
	LayoutDelegate* ld;
};

/////////////////////////////////////////

struct LAYOUT_LINE_RESULT {
	POINT point;
	const LayoutObject* lo;
};

struct INTERNAL_STATE {
	POINT left_point, right_point, current_point;
};

void layout(int viewport_width, const std::vector<Paragraph>& vector, std::vector<LAYOUT_LINE_RESULT>& result);

typedef std::vector<const LayoutObject*>::const_iterator LO_ITERATOR;

//A LayoutInternalState object holds data about a particular viewport- view contents
//configuration, to assist in efficient scrolling.
class LayoutInternalState {
protected:
	INTERNAL_STATE is;
	int viewport_width;
	std::vector<Paragraph>::const_iterator it, it_end;
	LO_ITERATOR it2;
	std::vector<LAYOUT_LINE_RESULT> result;
public:
	LayoutInternalState(int _viewport_width, const std::vector<Paragraph>& _vector);
	virtual ~LayoutInternalState();

	//scroll_position_y argument specifies the logical y-coordinate of the bottom of the
	//viewport; i.e. the furthest vertical extent of the document that is currently
	//visible.
	virtual void layout(int scroll_position_y);
	virtual const std::vector<LAYOUT_LINE_RESULT>& get_result() const;
};


/////////////////////////////////////////

class LeftJustifyingLayoutDelegate : public LayoutDelegate {
protected:
	unsigned x;

public:
	LeftJustifyingLayoutDelegate();
	virtual ~LeftJustifyingLayoutDelegate();
	virtual void start_new_line(unsigned left_extent, unsigned right_extent, unsigned line_width);
	virtual unsigned layout_line(const LayoutObject& lo);
};

LayoutDelegate* left_justifier();

class RightJustifyingLayoutDelegate : public LayoutDelegate {
protected:
	unsigned x;

public:
	RightJustifyingLayoutDelegate();
	virtual ~RightJustifyingLayoutDelegate();
	virtual void start_new_line(unsigned left_extent, unsigned right_extent, unsigned line_width);
	virtual unsigned layout_line(const LayoutObject& lo);
};

LayoutDelegate* right_justifier();

class CenterJustifyingLayoutDelegate : public LayoutDelegate {
protected:
	unsigned x;

public:
	CenterJustifyingLayoutDelegate();
	virtual ~CenterJustifyingLayoutDelegate();
	virtual void start_new_line(unsigned left_extent, unsigned right_extent, unsigned line_width);
	virtual unsigned layout_line(const LayoutObject& lo);
};

LayoutDelegate* center_justifier();

#endif