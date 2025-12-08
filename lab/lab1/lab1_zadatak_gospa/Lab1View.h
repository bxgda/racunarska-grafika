
// Lab1View.h : interface of the CLab1View class
//

#pragma once


#include <vector>
#include <cmath>

using namespace std;


class CLab1View : public CView
{
protected: // create from serialization only
	CLab1View() noexcept;
	DECLARE_DYNCREATE(CLab1View)

// Attributes
public:
	CLab1Doc* GetDocument() const;
	BOOL m_showGrid = TRUE;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CLab1View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void DrawRegularPolygon(CDC* pDC, int cx, int cy, int r, int n, float rotAngleDegrees);
	void DrawPolygon(CDC* pDC, const vector<POINT>& pts, COLORREF fillColor, int hatchStyle = -1, COLORREF hatchFg = RGB(0, 0, 0), COLORREF hatchBg = RGB(255, 255, 255));
	void DrawGrid(CDC* pDC, int left, int top, int width, int height, int spacing);
	double Distance(POINT a, POINT b);
	POINT CenterOfThreeDots(POINT a, POINT b, POINT c);
	POINT CenterOfThreeDots(vector<POINT> pts);
	POINT GridPoint(double gx, double gy);

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in Lab1View.cpp
inline CLab1Doc* CLab1View::GetDocument() const
   { return reinterpret_cast<CLab1Doc*>(m_pDocument); }
#endif

