
// gdi_2016View.h : interface of the Cgdi2016View class
#include "DImage.h"

#pragma once



class Cgdi2016View : public CView
{
protected: // create from serialization only
	Cgdi2016View() noexcept;
	DECLARE_DYNCREATE(Cgdi2016View)

// Attributes
public:
	Cgdi2016Doc* GetDocument() const;

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
	virtual ~Cgdi2016View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	// moje funkcije i promenljive ------------------------

protected:
	float ugaoStapa;
	float pomerajStapa;
	float pomerajKugle;
	float ugaoUdarca;

	DImage* zelenaPodloga;
	DImage* drveniOkvir;

	void DrawStick(CDC* pDC, int w);
	void DrawBall(CDC* pDC, int w);

	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply);
	void Rotate(CDC* pDC, float angle, bool rightMultiply);

	void DrawTable(CDC* pDC, const CRect& rect);
	void DrawBorder(CDC* pDC, CRect rect, int w);
	void DrawHoles(CDC* pDC, CRect rect, int size);

public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in gdi_2016View.cpp
inline Cgdi2016Doc* Cgdi2016View::GetDocument() const
   { return reinterpret_cast<Cgdi2016Doc*>(m_pDocument); }
#endif

