
// gdi_2014View.h : interface of the Cgdi2014View class
//

#pragma once
#include "DImage.h"


class Cgdi2014View : public CView
{
protected: // create from serialization only
	Cgdi2014View() noexcept;
	DECLARE_DYNCREATE(Cgdi2014View)

// Attributes
public:
	Cgdi2014Doc* GetDocument() const;

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
	virtual ~Cgdi2014View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	// moje dodatne promenljive i funkcije

protected:
	DImage* tocak;        
	HENHMETAFILE reno;

	float ugaoNagiba;
	float pozicijaAuta;

	void Scale(CDC* pDC, float sX, float sY, bool rightMultiply);
	void Rotate(CDC* pDC, float angle, bool rightMultiply);
	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply);

	void DrawGround(CDC* pDC, float angle);
	void DrawCar(CDC* pDC, int x, int y, int w, int h);
	void DrawWheel(CDC* pDC, int x, int y, int r, float angle);


public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in gdi_2014View.cpp
inline Cgdi2014Doc* Cgdi2014View::GetDocument() const
   { return reinterpret_cast<Cgdi2014Doc*>(m_pDocument); }
#endif

