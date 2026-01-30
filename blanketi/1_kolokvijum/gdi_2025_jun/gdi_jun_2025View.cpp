
// gdi_jun_2025View.cpp : implementation of the Cgdijun2025View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "gdi_jun_2025.h"
#endif

#include "gdi_jun_2025Doc.h"
#include "gdi_jun_2025View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TO_RAD 3.14159265 / 180.0f
#define TO_DEG 180.0f / 3.14159265


// Cgdijun2025View

IMPLEMENT_DYNCREATE(Cgdijun2025View, CView)

BEGIN_MESSAGE_MAP(Cgdijun2025View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// Cgdijun2025View construction/destruction

Cgdijun2025View::Cgdijun2025View() noexcept
{
	// TODO: add construction code here

}

Cgdijun2025View::~Cgdijun2025View()
{
}

BOOL Cgdijun2025View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// Cgdijun2025View drawing

void Cgdijun2025View::OnDraw(CDC* pDC)
{
	Cgdijun2025Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CRect rcClient;
	GetClientRect(&rcClient);

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	CBitmap memBmp;
	memBmp.CreateCompatibleBitmap(pDC, rcClient.Width(), rcClient.Height());
	CBitmap* pOldBmp = memDC.SelectObject(&memBmp);

	memDC.FillSolidRect(rcClient, RGB(255, 255, 255));

	int oldMode = memDC.SetGraphicsMode(GM_ADVANCED);
	XFORM oldXf;
	memDC.GetWorldTransform(&oldXf);

	DrawScene(&memDC);

	memDC.SetWorldTransform(&oldXf);
	memDC.SetGraphicsMode(oldMode);


	pDC->BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(pOldBmp);
}


// Cgdijun2025View printing

BOOL Cgdijun2025View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void Cgdijun2025View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void Cgdijun2025View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// Cgdijun2025View diagnostics

#ifdef _DEBUG
void Cgdijun2025View::AssertValid() const
{
	CView::AssertValid();
}

void Cgdijun2025View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

void Cgdijun2025View::DrawScene(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(&rcClient);

	POINT centar = { rcClient.right / 2, rcClient.bottom / 2 };

	DrawFerrisWheel(pDC, 200, 2, 20, RGB(23, 56, 78), centar, 90, CString("Festival"));
}

void Cgdijun2025View::DrawCoordinateSystem(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(&rcClient);

	CPen penX(PS_SOLID, 2, RGB(255, 0, 0));
	CPen penY(PS_SOLID, 2, RGB(0, 200, 0));

	CPen* oldPen = pDC->SelectObject(&penX);

	// X osa
	pDC->MoveTo(0, 0);
	pDC->LineTo(rcClient.right, 0);
	pDC->SelectObject(&penY);

	// Y osa
	pDC->MoveTo(0, 0);
	pDC->LineTo(0, rcClient.bottom);
	pDC->SelectObject(oldPen);
}

void Cgdijun2025View::Translate(CDC* pDC, float dX, float dY, bool rightMultiply)
{
	XFORM transMatrix;
	transMatrix.eM11 = 1;
	transMatrix.eM12 = 0;
	transMatrix.eM21 = 0;
	transMatrix.eM22 = 1;
	transMatrix.eDx = dX;
	transMatrix.eDy = dY;

	if (rightMultiply)
		pDC->ModifyWorldTransform(&transMatrix, MWT_RIGHTMULTIPLY);
	else
		pDC->ModifyWorldTransform(&transMatrix, MWT_LEFTMULTIPLY);
}

void Cgdijun2025View::Rotate(CDC* pDC, float angle, bool rightMultiply)
{
	float angleRad = angle * TO_RAD;
	XFORM transMatrix;
	transMatrix.eM11 = cos(angleRad);
	transMatrix.eM12 = sin(angleRad);
	transMatrix.eM21 = -sin(angleRad);
	transMatrix.eM22 = cos(angleRad);
	transMatrix.eDx = 0;
	transMatrix.eDy = 0;

	if (rightMultiply)
		pDC->ModifyWorldTransform(&transMatrix, MWT_RIGHTMULTIPLY);
	else
		pDC->ModifyWorldTransform(&transMatrix, MWT_LEFTMULTIPLY);
}

void Cgdijun2025View::DrawCabin(CDC* pDC, int w, int d, COLORREF clr)
{
	XFORM oldXf{};
	pDC->GetWorldTransform(&oldXf);

	CPen pen(PS_SOLID, 2, clr);
	CPen* pOldPen = pDC->SelectObject(&pen);

	CBrush brush(clr);
	CBrush* pOldBrush = pDC->SelectObject(&brush);

	pDC->MoveTo(0, 0);
	pDC->LineTo(0, w / 4);

	Translate(pDC, 0, w / 4);

	int levo =  - w / 2;
	int desno = w / 2;
	int gore = 0;
	int dole = w;

	pDC->Pie(levo, gore, desno, dole, desno, gore + w / 2, levo, gore + w / 2);

	pDC->MoveTo(-w / 2, w / 2);
	pDC->LineTo(-w / 2, w);

	pDC->MoveTo(w / 2, w / 2);
	pDC->LineTo(w / 2, w);

	COLORREF lightClr = RGB(
		(GetRValue(clr) + (255 - GetRValue(clr)) * 0.3),
		(GetGValue(clr) + (255 - GetGValue(clr)) * 0.3),
		(GetBValue(clr) + (255 - GetBValue(clr)) * 0.3)
	);

	CBrush drugaBrush(HS_DIAGCROSS, lightClr);
	pDC->SelectObject(&drugaBrush);

	int a = (w - (2 * w / 3)) / 2;

	POINT tacke[4] = {
		{-w / 2, w},
		{w / 2,w},
		{w / 2 - a, w + w / 2},
		{-w / 2 + a, w + w / 2}
	};

	pDC->Polygon(tacke, 4);

	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);

	pDC->SetWorldTransform(&oldXf);
}

void Cgdijun2025View::DrawWheelPetal(CDC* pDC, int r, int d, int n, COLORREF clr)
{
	XFORM oldXf{};
	pDC->GetWorldTransform(&oldXf);

	CPen deblja(PS_SOLID, 3 * d, clr);
	CPen* pOldPen = pDC->SelectObject(&deblja);

	CBrush brush(RGB(0, 255, 0));
	CBrush* pOldBrush = pDC->SelectObject(&brush);

	pDC->Ellipse(-r / 10, -r / 10, r / 10, r / 10);

	pDC->SelectStockObject(NULL_BRUSH);

	float manjeR = 4 * r / 5;

	pDC->Ellipse(-r, -r, r, r);
	pDC->Ellipse(-manjeR, -manjeR, manjeR, manjeR);

	CPen tanja(PS_SOLID, d, clr);
	pDC->SelectObject(&tanja);

	for (int i = 0; i < 360; i+= (360 / n))
	{
		pDC->Ellipse(-r / 10, -r, r / 10, r);
		Rotate(pDC, 360 / n);
	}

	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);

	pDC->SetWorldTransform(&oldXf);
}

void Cgdijun2025View::DrawFerrisWheel(CDC* pDC, int r, int d, int n, COLORREF clr, CPoint prCenter, int f, CString str)
{
	XFORM oldXf{};

	Translate(pDC, prCenter.x, prCenter.y);

	pDC->GetWorldTransform(&oldXf);

	CPen deblja(PS_SOLID, d, clr);
	CPen* pOldPen = pDC->SelectObject(&deblja);

	pDC->SelectStockObject(NULL_BRUSH);

	POINT tacke[3] = {
		{ 0, 0 },
		{ 0.4 * r, 1.6 * r },
		{ -0.4 * r, 1.6 * r }
	};

	pDC->Polygon(tacke, 3);

	Rotate(pDC, alpha);

	DrawWheelPetal(pDC, r, d, n, clr);

	for (int i = 0; i < 360; i += (360 / n) * 2)
	{
		Translate(pDC, 0, -r);
		
		Rotate(pDC, -i - alpha);
		DrawCabin(pDC, r / 5, d, clr);
		Rotate(pDC, i + alpha);
		Translate(pDC, 0, r);
		Rotate(pDC, 360 / n * 2);
	}

	pDC->SetWorldTransform(&oldXf);

	// ko voli nek ispise tekst ja ne mogu vise da gledam u ovo
}

Cgdijun2025Doc* Cgdijun2025View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cgdijun2025Doc)));
	return (Cgdijun2025Doc*)m_pDocument;
}
#endif //_DEBUG


// Cgdijun2025View message handlers

BOOL Cgdijun2025View::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
}

void Cgdijun2025View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_LEFT:
		alpha -= 5;
		break;
	case VK_RIGHT:
		alpha += 5;
		break;
	}
	Invalidate();

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
