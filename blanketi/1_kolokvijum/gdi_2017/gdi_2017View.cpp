
// gdi_2017View.cpp : implementation of the Cgdi2017View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "gdi_2017.h"
#endif

#include "gdi_2017Doc.h"
#include "gdi_2017View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TO_RAD 3.14159265 / 180.0f
#define TO_DEG 180.0f / 3.14159265


// Cgdi2017View

IMPLEMENT_DYNCREATE(Cgdi2017View, CView)

BEGIN_MESSAGE_MAP(Cgdi2017View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// Cgdi2017View construction/destruction

Cgdi2017View::Cgdi2017View() noexcept
{
	indeksKruga = 0;

	ugaoRotacijeZadnjeNoge = 20.0f;
	ugaoRotacijePrednjeNoge = 0.0f;
	predjeniPut = 0.0f;
	skaliranje = 1.0f;

	noga1 = new DImage;
	noga2 = new DImage;
	noga3 = new DImage;
	telo = new DImage;
	pozadina = new DImage;

	noga1->Load(CString("Leg1.png"));
	noga2->Load(CString("Leg2.png"));
	noga3->Load(CString("Leg3.png"));
	telo->Load(CString("Body.png"));
	pozadina->Load(CString("Back2.jpg"));
}

Cgdi2017View::~Cgdi2017View()
{
	if (noga1)
		delete noga1;
	if (noga2)
		delete noga2;
	if (noga3)
		delete noga3;
	if (telo)
		delete telo;
	if (pozadina)
		delete pozadina;
}

BOOL Cgdi2017View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// Cgdi2017View drawing

void Cgdi2017View::OnDraw(CDC* pDC)
{
	// anti flicker ---------------------------
	CRect pravougaonik;
	GetClientRect(&pravougaonik);

	CDC* pMemDC = new CDC();
	pMemDC->CreateCompatibleDC(pDC);
	CBitmap bpm;
	bpm.CreateCompatibleBitmap(pDC, pravougaonik.Width(), pravougaonik.Height());
	pMemDC->SelectObject(&bpm);
	// ----------------------------------------

	DrawScene(pMemDC, pravougaonik);

	// kraj antiflicker-a
	pDC->BitBlt(0, 0, pravougaonik.Width(), pravougaonik.Height(), pMemDC, 0, 0, SRCCOPY);
	pMemDC->DeleteDC();
	delete pMemDC;
}


// Cgdi2017View printing

BOOL Cgdi2017View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void Cgdi2017View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void Cgdi2017View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// Cgdi2017View diagnostics

#ifdef _DEBUG
void Cgdi2017View::AssertValid() const
{
	CView::AssertValid();
}

void Cgdi2017View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

void Cgdi2017View::LoadIdentity(CDC* pDC)
{
	// ovo samo sluzi da bi resetovali sve transformacije

	pDC->SetGraphicsMode(GM_ADVANCED);

	XFORM xf{};
	xf.eM11 = 1.0f; xf.eM12 = 0.0f;
	xf.eM21 = 0.0f; xf.eM22 = 1.0f;
	xf.eDx = 0.0f; xf.eDy = 0.0f;

	pDC->SetWorldTransform(&xf);
}

void Cgdi2017View::Translate(CDC* pDC, float dX, float dY, bool rightMultiply)
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

void Cgdi2017View::Rotate(CDC* pDC, float angle, bool rightMultiply)
{
	XFORM transMatrix;
	transMatrix.eM11 = cos(angle);
	transMatrix.eM12 = sin(angle);
	transMatrix.eM21 = -sin(angle);
	transMatrix.eM22 = cos(angle);
	transMatrix.eDx = 0;
	transMatrix.eDy = 0;

	if (rightMultiply)
		pDC->ModifyWorldTransform(&transMatrix, MWT_RIGHTMULTIPLY);
	else
		pDC->ModifyWorldTransform(&transMatrix, MWT_LEFTMULTIPLY);
}

void Cgdi2017View::Scale(CDC* pDC, float sX, float sY, bool rightMultiply)
{
	XFORM transMatrix;
	transMatrix.eM11 = sX;
	transMatrix.eM12 = 0;
	transMatrix.eM21 = 0;
	transMatrix.eM22 = sY;
	transMatrix.eDx = 0;
	transMatrix.eDy = 0;

	if (rightMultiply)
		pDC->ModifyWorldTransform(&transMatrix, MWT_RIGHTMULTIPLY);
	else
		pDC->ModifyWorldTransform(&transMatrix, MWT_LEFTMULTIPLY);
}

void Cgdi2017View::DrawScene(CDC* pDC, CRect rect)
{
	CRect src(0, 0, pozadina->Width(), pozadina->Height());
	pozadina->Draw(pDC, src, rect);

	pDC->SetGraphicsMode(GM_ADVANCED);

	XFORM oldXf{};
	pDC->GetWorldTransform(&oldXf);

	CRect teloSrc(0, 0, telo->Width(), telo->Height());
	CRect teloDst(100, 100, 100 + telo->Width(), 100 + telo->Height());

	Scale(pDC, skaliranje, skaliranje, false);

	Translate(pDC, predjeniPut, 0.0f, false);

	telo->DrawTransparent(pDC, teloSrc, teloDst, RGB(255, 255, 255));

	DrawLeg(pDC, ugaoRotacijeZadnjeNoge, 100 + 54, 100 + 175);
	DrawLeg(pDC, -ugaoRotacijeZadnjeNoge, 100 + 54, 100 + 175);

	DrawLeg(pDC, ugaoRotacijePrednjeNoge, 100 + 270, 100 + 175);
	DrawLeg(pDC, -ugaoRotacijePrednjeNoge, 100 + 270, 100 + 175);

	pDC->SetWorldTransform(&oldXf);
}

void Cgdi2017View::DrawLeg(CDC* pDC, double alpha, double dx, double dy)
{
	XFORM oldXf{};
	pDC->GetWorldTransform(&oldXf);

	Translate(pDC, dx, dy, false);

	Translate(pDC, 28, 28, false);
	Rotate(pDC, (float)(alpha * TO_RAD), false);
	Translate(pDC, -28, -28, false);

	CRect noga1Src(0, 0, noga1->Width(), noga1->Height());
	CRect noga1Dst(0, 0, noga1->Width(), noga1->Height());
	noga1->DrawTransparent(pDC, noga1Src, noga1Dst, RGB(255, 255, 255));

	Translate(pDC, 0, noga1->Height() - 40, false);

	Translate(pDC, 29, 29, false);
	Rotate(pDC, (float)(-alpha * TO_RAD), false);
	Translate(pDC, -29, -29, false);

	CRect noga2Src(0, 0, noga2->Width(), noga2->Height());
	CRect noga2Dst(0, 0, noga2->Width(), noga2->Height());
	noga2->DrawTransparent(pDC, noga2Src, noga2Dst, RGB(255, 255, 255));

	Translate(pDC, 0 - 35, noga2->Height() - 28, false);

	CRect noga3Src(0, 0, noga3->Width(), noga3->Height());
	CRect noga3Dst(0, 0, noga3->Width(), noga3->Height());
	noga3->DrawTransparent(pDC, noga3Src, noga3Dst, RGB(255, 255, 255));

	pDC->SetWorldTransform(&oldXf);
}

Cgdi2017Doc* Cgdi2017View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cgdi2017Doc)));
	return (Cgdi2017Doc*)m_pDocument;
}
#endif //_DEBUG


// Cgdi2017View message handlers

void Cgdi2017View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// jebem ti ja ovu matematiku

	// ciklus:
	// -20, -19, ..., -1, 0, 1, ..., 19, 20, 19, ..., 1, 0, -1, ..., -19, -20

	const int maxUgao = 20;
	const int duzinaCiklusa = 2 * (2 * maxUgao) + 1; // 81 (raste 40 koraka pa opada 40 koraka)

	if (nChar == VK_RIGHT)
	{
		indeksKruga = (indeksKruga + 1) % duzinaCiklusa;
		predjeniPut += 3.0f;
	}
	else if (nChar == VK_LEFT)
	{
		indeksKruga = (indeksKruga - 1 + duzinaCiklusa) % duzinaCiklusa;
		predjeniPut -= 3.0f;
	}
	else if (nChar == VK_UP)
	{
		skaliranje += 0.1f;
		Invalidate();
		CView::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}
	else if (nChar == VK_DOWN)
	{
		if (skaliranje >= 1.0f)
			skaliranje -= 0.1f;

		Invalidate();
		CView::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}
	else
	{
		CView::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}

	// mapiranje indeksa 0..80 u ugao -20..20..-20
	// prvo mapiramo u t = 0..40..0 (trougao), pa od toga oduzmemo 20
	int t = indeksKruga;
	if (t > 2 * maxUgao)
		t = 4 * maxUgao - t; // 80 - t

	const int ugaoZadnje = t - maxUgao;

	// prednje noge: isti ciklus, ali fazno pomeren za 20 "stepeni" (20 koraka)
	const int fazniPomeraj = 20;
	int indeksPrednje = (indeksKruga + fazniPomeraj) % duzinaCiklusa;

	int tPrednje = indeksPrednje;

	if (tPrednje > 2 * maxUgao)
		tPrednje = 4 * maxUgao - tPrednje;

	const int ugaoPrednje = tPrednje - maxUgao;

	ugaoRotacijeZadnjeNoge = (float)ugaoZadnje;
	ugaoRotacijePrednjeNoge = (float)ugaoPrednje;

	Invalidate();

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL Cgdi2017View::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
}
