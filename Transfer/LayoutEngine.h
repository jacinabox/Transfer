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
	virtual void start_new_line(unsigned left_extent, unsigned right_extent) = 0;
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

typedef std::vector<const LayoutObject*>::const_iterator LO_ITERATOR;

void layout(int viewport_width, const std::vector<Paragraph>& vector, std::vector<LAYOUT_LINE_RESULT>& result);

/////////////////////////////////////////

class LeftJustifyingLayoutDelegate : public LayoutDelegate {
protected:
	unsigned x;

public:
	LeftJustifyingLayoutDelegate();
	virtual ~LeftJustifyingLayoutDelegate();
	virtual void start_new_line(unsigned left_extent, unsigned right_extent);
	virtual unsigned layout_line(const LayoutObject& lo);
};

LayoutDelegate* left_justifier();

#endif