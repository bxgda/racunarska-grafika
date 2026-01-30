
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

// Metode i atributi ------------------------------------------------------------------------------
protected:
	std::stack<XFORM> m_transformStack;

	DImage* m_base;
	DImage* m_arm1;
	DImage* m_arm2;
	DImage* m_head;
	DImage* m_pozadina;
	DImage* m_baseShadow;
	DImage* m_arm1Shadow;
	DImage* m_arm2Shadow;
	DImage* m_headShadow;

	int m_rotArm1 = 0;
	int m_rotArm2 = 0;
	int m_rotHead = 0;

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

	void DrawLampBase(CDC* pDC, bool blsShadow);
	void DrawLampArm1(CDC* pDC, bool bIsShadow);
	void DrawLampArm2(CDC* pDC, bool bIsShadow);
	void DrawLampHead(CDC* pDC, bool bIsShadow);
	void DrawLamp(CDC* pDC, bool bIsShadow);
	void DrawLampShadow(CDC* pDC);
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

