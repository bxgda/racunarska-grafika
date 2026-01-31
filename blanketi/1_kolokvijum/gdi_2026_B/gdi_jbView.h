
// gdi_jbView.h : interface of the CgdijbView class
//

#pragma once

#include "DImage.h"


class CgdijbView : public CView
{
protected: // create from serialization only
	CgdijbView() noexcept;
	DECLARE_DYNCREATE(CgdijbView)

// Attributes
public:
	CgdijbDoc* GetDocument() const;

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
	virtual ~CgdijbView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	// moje promenljive u funkcije ------------------------------------------------------------------------

	float ugaoRampa1 = 0.0f;
	float ugaoRampa2 = 0.0f;

	int brojPrecki = 8;

	bool flagSvetlo = false;
	bool svetla[2] = { false, false };

	DImage* pozadina;
	DImage* pole1;
	DImage* pole2;
	DImage* barrierB;
	DImage* stripB;

	void DrawScene(CDC* pDC);

	void DrawBackground(CDC* pDC);
	void DrawImageTransparent(CDC* pDC, DImage* pImage);

	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply = false);
	void Rotate(CDC* pDC, float angle, bool rightMultiply = false);
	void Scale(CDC* pDC, float sX, float sY, bool rightMultiply = false);

	void DrawTrafficSign(CDC* pDC, int a, int h, int n, bool light[]);
	void DrawStrip(CDC* pDC, CString s);
	void DrawBarrierBoom(CDC* pDC, CString s, int nStrip, double angle);

	void DrawCoordinateSystem(CDC* pDC);
	void DrawRailwayBarrier(CDC* pDC, CString s, int nStrip, double alpha, double beta);

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	// --------------------------------------------------------------------
};

#ifndef _DEBUG  // debug version in gdi_jbView.cpp
inline CgdijbDoc* CgdijbView::GetDocument() const
   { return reinterpret_cast<CgdijbDoc*>(m_pDocument); }
#endif

