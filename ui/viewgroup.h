#pragma once

#include "ui/view.h"
#include "input/gesture_detector.h"

namespace UI {

class ViewGroup : public View {
public:
	ViewGroup(LayoutParams *layoutParams = 0) : View(layoutParams) {}
	virtual ~ViewGroup();

	// Pass through external events to children.
	virtual void Touch(const TouchInput &input);

	// By default, a container will layout to its own bounds.
	virtual void Measure(const DrawContext &dc, MeasureSpec horiz, MeasureSpec vert) = 0;
	virtual void Layout() = 0;
	virtual void Update(const InputState &input_state);

	virtual void Draw(DrawContext &dc);

	// These should be unused.
	virtual float GetContentWidth() const { return 0.0f; }
	virtual float GetContentHeight() const { return 0.0f; }

	// Takes ownership! DO NOT add a view to multiple parents!
	void Add(View *view) { views_.push_back(view); }

	virtual bool SetFocus();
	virtual void MoveFocus(FocusDirection direction);

	// Assumes that layout has taken place.
	View *FindNeighbor(View *view, FocusDirection direction);
	
	virtual bool CanBeFocused() const { return false; }

protected:
	std::vector<View *> views_;
};

// A frame layout contains a single child view (normally).
class FrameLayout : public ViewGroup {
public:
	void Measure(const DrawContext &dc, MeasureSpec horiz, MeasureSpec vert);
	void Layout();
};

class RelativeLayout : public ViewGroup {
public:
	void Measure(const DrawContext &dc, MeasureSpec horiz, MeasureSpec vert);
	void Layout();
};

class LinearLayout : public ViewGroup {
public:
	LinearLayout(Orientation orientation, LayoutParams *layoutParams = 0)
		: ViewGroup(layoutParams), spacing_(5), orientation_(orientation), defaultMargins_(0) {}

	void Measure(const DrawContext &dc, MeasureSpec horiz, MeasureSpec vert);
	void Layout();

private:
	Orientation orientation_;
	Margins defaultMargins_;
	float spacing_;
};

class GridLayout : public ViewGroup {
public:
	GridLayout(Orientation orientation, int numPerLine, LayoutParams *layoutParams = 0)
		: ViewGroup(layoutParams), orientation_(orientation), numPerLine_(numPerLine) {}

	void Measure(const DrawContext &dc, MeasureSpec horiz, MeasureSpec vert);
	void Layout();

private:
	Orientation orientation_;
	int numPerLine_;
};

// A scrollview usually contains just a single child - a linear layout or similar.
class ScrollView : public ViewGroup {
public:
	ScrollView(Orientation orientation, LayoutParams *layoutParams = 0) :
		ViewGroup(layoutParams), orientation_(orientation), scrollPos_(0) {}

	void Measure(const DrawContext &dc, MeasureSpec horiz, MeasureSpec vert);
	void Layout();

	void Touch(const TouchInput &input);
	void Draw(DrawContext &dc);

private:
	GestureDetector gesture_;
	Orientation orientation_;
	float scrollPos_;
	float scrollStart_;
};

class ViewPager : public ScrollView {
public:
};

void LayoutViewHierarchy(const DrawContext &dc, ViewGroup *root);
void UpdateViewHierarchy(const InputState &input_state, ViewGroup *root);

}  // namespace UI