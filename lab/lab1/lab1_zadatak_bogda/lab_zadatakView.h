
// lab_zadatakView.h : interface of the ClabzadatakView class
//

#pragma once


class ClabzadatakView : public CView
{
protected: // create from serialization only
	ClabzadatakView() noexcept;
	DECLARE_DYNCREATE(ClabzadatakView)

// Attributes
public:
	ClabzadatakDoc* GetDocument() const;

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
	virtual ~ClabzadatakView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	// moje dodatne funkcije i promenljive
protected:
	bool mreza;

	void NacrtajSveOblike(CDC* pDC);

	void SivaPozadina(CDC* pDC, CPen*& oldPen, CBrush*& oldBrush);
	void NacrtajMrezu(CDC* pDC);

	POINT Tacka(float brKockicaX, float brKockicaY);

	POINT VratiCentarTrougla(POINT* temena);
	double Razdaljina(POINT A, POINT B);

	void SelektujOlovkuiCetkicu(CDC* pDC, CPen& pen, CBrush& brush, CPen*& oldPen, CBrush*& oldBrush);
	void VratiOlovkuiCetkicu(CDC* pDC, CPen* oldPen, CBrush* oldBrush);

	void NacrtajOblik(CDC* pDC, POINT* temena, int brTemena,
		COLORREF bojaOlovke, COLORREF bojaCetke,
		int tipSrafure,
		int polyN, int polyR, float polyRot);

	void DrawRegularPolygon(CDC* pDC, int cx, int cy, int r, int n, float rotAngle);

public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // debug version in lab_zadatakView.cpp
inline ClabzadatakDoc* ClabzadatakView::GetDocument() const
   { return reinterpret_cast<ClabzadatakDoc*>(m_pDocument); }
#endif

