
// gdi_jun_2025View.h : interface of the Cgdijun2025View class
//

#pragma once


class Cgdijun2025View : public CView
{
protected: // create from serialization only
	Cgdijun2025View() noexcept;
	DECLARE_DYNCREATE(Cgdijun2025View)

// Attributes
public:
	Cgdijun2025Doc* GetDocument() const;

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
	virtual ~Cgdijun2025View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	// moje promenljive i funkcije --------------------------

	int alpha = 0; // ugao rotacije tocka

	void DrawScene(CDC* pDC);
	void DrawCoordinateSystem(CDC* pDC);

	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply = false);
	void Rotate(CDC* pDC, float angle, bool rightMultiply = false);
	void DrawCabin(CDC* pDC, int w, int d, COLORREF clr);
	void DrawWheelPetal(CDC* pDC, int r, int d, int n, COLORREF clr);
	void DrawFerrisWheel(CDC* pDC, int r, int d, int n, COLORREF clr, CPoint prCenter, int f, CString str);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in gdi_jun_2025View.cpp
inline Cgdijun2025Doc* Cgdijun2025View::GetDocument() const
   { return reinterpret_cast<Cgdijun2025Doc*>(m_pDocument); }
#endif

