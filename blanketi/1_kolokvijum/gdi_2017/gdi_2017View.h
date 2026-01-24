
// gdi_2017View.h : interface of the Cgdi2017View class
//

#pragma once

#include "DImage.h"


class Cgdi2017View : public CView
{
protected: // create from serialization only
	Cgdi2017View() noexcept;
	DECLARE_DYNCREATE(Cgdi2017View)

// Attributes
public:
	Cgdi2017Doc* GetDocument() const;

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
	virtual ~Cgdi2017View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	// moje dodatne promenljive i funkcije ----------------------------

	DImage* noga1;
	DImage* noga2;
	DImage* noga3;
	DImage* telo;
	DImage* pozadina;

	int indeksKruga; // koristi se za animaciju nogu

	float ugaoRotacijeZadnjeNoge;
	float ugaoRotacijePrednjeNoge;
	float predjeniPut;
	float skaliranje;	

	void LoadIdentity(CDC* pDC);
	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply);
	void Rotate(CDC* pDC, float angle, bool rightMultiply);
	void Scale(CDC* pDC, float sX, float sY, bool rightMultiply);

	void DrawScene(CDC* pDC, CRect rect);
	void DrawLeg(CDC* pDC, double alpha, double dx, double dy);
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
	// potrebno takodje za antiflicker kako stvarno ne bi treperelo
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#ifndef _DEBUG  // debug version in gdi_2017View.cpp
inline Cgdi2017Doc* Cgdi2017View::GetDocument() const
   { return reinterpret_cast<Cgdi2017Doc*>(m_pDocument); }
#endif


