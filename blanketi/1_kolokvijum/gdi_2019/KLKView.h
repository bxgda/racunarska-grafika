
// KLKView.h : interface of the CKLKView class
//

#pragma once

#include <stack>

#define PI 3.14159265358979f

class DImage;

class CKLKView : public CView
{
protected: // create from serialization only
	CKLKView() noexcept;
	DECLARE_DYNCREATE(CKLKView)

// Attributes
public:
	CKLKDoc* GetDocument() const;

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
	virtual ~CKLKView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

/*
	uzasan zadatak, lose smiljen kako treba da se radi pa ako se sktrogo prate njihove funkcije samo je teze
	lose zadati brojevi i sve vreme se "nabadaju ti brojevi" sve u svemu lose
*/


// Metode i atributi ------------------------------------------------------------------------------
protected:
	std::stack<XFORM> m_transformStack;

	DImage* m_body1;
	DImage* m_arm1;
	DImage* m_arm2;
	DImage* m_leg1;
	DImage* m_leg2;
	DImage* m_background;

	float m_rotArm1 = 0;
	float m_rotArm2 = 0;

	float m_rotLeg1 = 0;
	float m_rotLeg2 = 0;

	float m_rotBody1 = 0;

public:
	void Push(CDC* pDC);
	void Pop(CDC* pDC);
	void DrawCoordinateSystem(CDC* pDC);
	inline float ToRad(float degrees) { return degrees * PI / 180.0f; }
	

	void Translate(CDC* pDC, float dx, float dy, bool rightMultiply = false);
	void Rotate(CDC* pDC, float angle, bool rightMultiply = false);
	void Scale(CDC* pDC, float sx, float sy, bool rightMultiply = false);
	void Mirror(CDC* pDC, bool mx, bool my, bool rightMultiply = false);
	
	void DrawBackground(CDC* pDC);
	void DrawImageTransparent(CDC* pDC, DImage* pImage);
	void DrawScene(CDC* pDC);

	void DrawArm1(CDC* pDC);
	void DrawArm2(CDC* pDC);

	void DrawLeg1(CDC* pDC);
	void DrawLeg2(CDC* pDC);

	void DrawBody1(CDC* pDC);

	void DrawTransformer(CDC* pDC);
// ------------------------------------------------------------------------------------------------

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in KLKView.cpp
inline CKLKDoc* CKLKView::GetDocument() const
   { return reinterpret_cast<CKLKDoc*>(m_pDocument); }
#endif

