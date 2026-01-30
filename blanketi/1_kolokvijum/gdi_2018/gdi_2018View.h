
// gdi_2018View.h : interface of the Cgdi2018View class
//

#pragma once
#include "DImage.h"


class Cgdi2018View : public CView
{
protected: // create from serialization only
	Cgdi2018View() noexcept;
	DECLARE_DYNCREATE(Cgdi2018View)

// Attributes
public:
	Cgdi2018Doc* GetDocument() const;

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
	virtual ~Cgdi2018View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	//moje funkcije i promenljive  ------------------------------------------

protected:
	DImage* bager;
	DImage* arm1;
	DImage* arm2;
	DImage* pozadina;
	HENHMETAFILE viljuska;
	CRect viljuskaRect;

	float ugao1 = -90.0f;
	float ugao2 = -90.0f;
	float ugao3 = 0.0f;

	int predjeniPut = 0;

	void Scale(CDC* pDC, float sX, float sY, bool rightMultiply = false);
	void Rotate(CDC* pDC, float angle, bool rightMultiply = false);
	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply = false);

	void DrawBackground(CDC* pDC);
	void DrawImgTransparent(CDC* pDC, DImage* pImage);
	void DrawBody(CDC* pDC);
	void DrawArm1(CDC* pDC);
	void DrawArm2(CDC* pDC);
	void DrawFork(CDC* pDC);
	void DrawExcavator(CDC* pDC);
	
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	// --------------------------------------------------------------------
};


#ifndef _DEBUG  // debug version in gdi_2018View.cpp
inline Cgdi2018Doc* Cgdi2018View::GetDocument() const
   { return reinterpret_cast<Cgdi2018Doc*>(m_pDocument); }
#endif

