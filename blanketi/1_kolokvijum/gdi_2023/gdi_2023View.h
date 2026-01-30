
// gdi_2023View.h : interface of the Cgdi2023View class
//

#pragma once
#include "DImage.h"


class Cgdi2023View : public CView
{
protected: // create from serialization only
	Cgdi2023View() noexcept;
	DECLARE_DYNCREATE(Cgdi2023View)

// Attributes
public:
	Cgdi2023Doc* GetDocument() const;

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
	virtual ~Cgdi2023View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	// moje promenljive u funkcije ---------------------------------------------

protected:
	DImage* glava;
	DImage* nadkolenica;
	DImage* nadlaktica;
	DImage* potkolenica;
	DImage* podlaktica;
	DImage* saka;
	DImage* stopalo;
	DImage* telo;
	DImage* pozadina;

	float ugaoNadlaktica = 0;
	float ugaoPodlaktica = 0;
	float ugaoSaka = 0;
	float ugaoCeoRobot = 0;
	float skaliranjeRobota = 1;

	void DrawCoordinateSystem(CDC* pDC);

	void DrawScene(CDC* pDC);
	void DrawBackground(CDC* pDC);
	void DrawImgTransparent(CDC* pDC, DImage* pImage);

	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply = false);
	void Rotate(CDC* pDC, float angle, bool rightMultiply = false);
	void Scale(CDC* pDC, float sX, float sY, bool rightMultiply = false);

	void DrawHalf(CDC* pDC);
	void DrawHead(CDC* pDC);
	void DrawRobot(CDC* pDC);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	// --------------------------------------------------------------------------
};

#ifndef _DEBUG  // debug version in gdi_2023View.cpp
inline Cgdi2023Doc* Cgdi2023View::GetDocument() const
   { return reinterpret_cast<Cgdi2023Doc*>(m_pDocument); }
#endif

