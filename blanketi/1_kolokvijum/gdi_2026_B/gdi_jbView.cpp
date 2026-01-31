
// gdi_jbView.cpp : implementation of the CgdijbView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "gdi_jb.h"
#endif

#include "gdi_jbDoc.h"
#include "gdi_jbView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TO_RAD 3.14 / 180.0f
#define TO_DEG 180.0f / 3.14


// CgdijbView

IMPLEMENT_DYNCREATE(CgdijbView, CView)

BEGIN_MESSAGE_MAP(CgdijbView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CgdijbView construction/destruction

CgdijbView::CgdijbView() noexcept
{
	pozadina = new DImage();
	pole1 = new DImage();
	pole2 = new DImage();
	barrierB = new DImage();
	stripB = new DImage();

	pozadina->Load(CString("Background.jpg"));
	pole1->Load(CString("Pole1.bmp"));
	pole2->Load(CString("Pole2.bmp"));
	barrierB->Load(CString("BarrierB.bmp"));
	stripB->Load(CString("StripB.bmp"));
}

CgdijbView::~CgdijbView()
{
	delete pozadina;
	delete pole1;
	delete pole2;
	delete barrierB;
	delete stripB;
}

BOOL CgdijbView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CgdijbView drawing

void CgdijbView::OnDraw(CDC* pDC)
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

	XFORM staraTransformacija{};
	pMemDC->GetWorldTransform(&staraTransformacija);

	pMemDC->SetGraphicsMode(GM_ADVANCED);

	DrawScene(pMemDC);

	pMemDC->SetWorldTransform(&staraTransformacija);

	// kraj antiflicker-a ---------------------------------------------
	pDC->BitBlt(0, 0, pravougaonik.Width(), pravougaonik.Height(), pMemDC, 0, 0, SRCCOPY);
	pMemDC->DeleteDC();
	delete pMemDC;
}


// CgdijbView printing

BOOL CgdijbView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CgdijbView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CgdijbView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CgdijbView diagnostics

#ifdef _DEBUG
void CgdijbView::AssertValid() const
{
	CView::AssertValid();
}

void CgdijbView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

void CgdijbView::DrawScene(CDC* pDC)
{
	DrawBackground(pDC);

	CRect prozor;
	GetClientRect(&prozor);

	XFORM oldf{};
	pDC->GetWorldTransform(&oldf);

	Translate(pDC, prozor.Width() / 2 + 550, prozor.Height() - 50 - 320);

	DrawTrafficSign(pDC, 160, 320, 4, svetla);

	pDC->SetWorldTransform(&oldf);

	Translate(pDC, prozor.Width() / 2 + 500, prozor.Height() - 25);

	DrawRailwayBarrier(pDC, CString("X"), brojPrecki, ugaoRampa1, ugaoRampa2);
}

void CgdijbView::DrawBackground(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);

	int imgW = pozadina->Width();
	int imgH = pozadina->Height();

	int x = (rc.Width() - imgW) / 2 + 100;
	int y = rc.Height() - imgH;

	CRect rcImg(0, 0, imgW, imgH);
	CRect rcDC(x, y, x + imgW, y + imgH);
	pozadina->Draw(pDC, rcImg, rcDC);
}

void CgdijbView::DrawImageTransparent(CDC* pDC, DImage* pImage)
{
	const int imgW = pImage->Width();
	const int imgH = pImage->Height();

	unsigned char* pBits = pImage->GetDIBBits();
	if (!pBits) return;

	COLORREF transparent = RGB(pBits[2], pBits[1], pBits[0]);

	CRect rcImg(0, 0, imgW, imgH);
	CRect rcDC(0, 0, imgW, imgH);

	pImage->DrawTransparent(pDC, rcImg, rcDC, transparent);
}

void CgdijbView::Translate(CDC* pDC, float dX, float dY, bool rightMultiply)
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

void CgdijbView::Rotate(CDC* pDC, float angle, bool rightMultiply)
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

void CgdijbView::Scale(CDC* pDC, float sX, float sY, bool rightMultiply)
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

void CgdijbView::DrawTrafficSign(CDC* pDC, int a, int h, int n, bool light[])
{
	CPen olovka(PS_SOLID, 1, RGB(64, 128, 255));
	CPen* staraOlovka = pDC->SelectObject(&olovka);

	CBrush cetka(RGB(64, 128, 255));
	CBrush* staraCetka = pDC->SelectObject(&cetka);

	POINT tackeStub[4] = {
		{-0.15 * a / 2, 0},
		{0.15 * a / 2, 0},
		{0.15 * a / 2, h},
		{-0.15 * a / 2, h}
	};

	pDC->Polygon(tackeStub, 4);

	float visinaS = 0.2 * h / n;
	float ukoseno = visinaS / 2;

	float pomerajS = h / n;

	XFORM oldf{};
	pDC->GetWorldTransform(&oldf);

	POINT tackeS[4] = {
		{-0.15 * a / 2, 0},
		{-0.15 * a / 2, visinaS},
		{0.15 * a / 2, visinaS + ukoseno},
		{0.15 * a / 2, 0 + ukoseno},
	};

	CBrush zuta(RGB(255, 192, 96));
	pDC->SelectObject(&zuta);

	for (int i = 0; i < n; i++)
	{
		pDC->Polygon(tackeS, 4);
		Translate(pDC, 0, pomerajS);
	}

	pDC->SetWorldTransform(&oldf);

	CPen znak(PS_SOLID, 0.1 * a, RGB(255, 64, 64));
	pDC->SelectObject(&znak);

	float visinaTrougla = sqrt(pow(a, 2) - pow(a / 2, 2));

	CPoint tackeZ[3]{
		{a / 2, 0},
		{-a / 2, 0},
		{0, -(int)visinaTrougla}
	};

	pDC->Polygon(tackeZ, 3);

	COLORREF on(RGB(192, 48, 48));
	COLORREF off(RGB(255, 64, 64));

	CBrush upaljenoB(on);
	CBrush ugasenoB(off);

	CPen upaljenoP(PS_SOLID, 1, on);
	CPen ugasenoP(PS_SOLID, 1, off);

	pDC->SelectObject(&olovka);

	float r = 0.14 * a;
	float centarX = 1.1 * r;
	float centarY = 1.6 * r;

	if (!light[0])
	{
		pDC->SelectObject(&upaljenoB);
		pDC->SelectObject(&upaljenoP);
	}
	else
	{
		pDC->SelectObject(&ugasenoB);
		pDC->SelectObject(&ugasenoP);
	}

	pDC->Ellipse(-centarX - r, -centarY - r, -centarX + r, -centarY + r);

	if (!light[1])
	{
		pDC->SelectObject(&upaljenoB);
		pDC->SelectObject(&upaljenoP);
	}
	else
	{
		pDC->SelectObject(&ugasenoB);
		pDC->SelectObject(&ugasenoP);
	}

	pDC->Ellipse(centarX - r, -centarY - r, centarX + r, -centarY + r);

	pDC->SelectObject(&staraCetka);
	pDC->SelectObject(&staraOlovka);
}

void CgdijbView::DrawStrip(CDC* pDC, CString s)
{
	XFORM oldf{};
	pDC->GetWorldTransform(&oldf);

	Rotate(pDC, -90);
	Translate(pDC, -stripB->Width(), -stripB->Height() / 2);

	DrawImageTransparent(pDC, stripB);

	CFont font;
	font.CreateFont(
		-24, 
		0,
		0,
		0,
		FW_BOLD,
		FALSE,
		FALSE,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		_T("Arial"));

	CFont* pOldFont = pDC->SelectObject(&font);

	const int oldBkMode = pDC->SetBkMode(TRANSPARENT);
	const COLORREF oldTextColor = pDC->SetTextColor(RGB(128, 0, 0));
	const UINT oldAlign = pDC->SetTextAlign(TA_BOTTOM);

	Rotate(pDC, 90);

	pDC->TextOut(16 / 2, 0, s);

	pDC->SetTextAlign(oldAlign);
	pDC->SetTextColor(oldTextColor);
	pDC->SetBkMode(oldBkMode);

	pDC->SelectObject(pOldFont);

	pDC->SetWorldTransform(&oldf);
}

void CgdijbView::DrawBarrierBoom(CDC* pDC, CString s, int nStrip, double angle)
{
	XFORM oldf{};
	pDC->GetWorldTransform(&oldf);

	Translate(pDC, -870, -barrierB->Height() / 2);

	DrawImageTransparent(pDC, barrierB);

	float pomeraj = 700 / nStrip;

	Translate(pDC, 100, barrierB->Height() / 2);

	for (int i = 100; i <= 800; i += pomeraj)
	{
		Rotate(pDC, -angle);
		DrawStrip(pDC, s);
		Rotate(pDC, angle);
		Translate(pDC, pomeraj, 0);
	}

	pDC->SetWorldTransform(&oldf);
}

void CgdijbView::DrawCoordinateSystem(CDC* pDC)
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

void CgdijbView::DrawRailwayBarrier(CDC* pDC, CString s, int nStrip, double alpha, double beta)
{
	XFORM oldf{};
	pDC->GetWorldTransform(&oldf);

	Rotate(pDC, -90);
	Translate(pDC, 0, -pole1->Height() / 2);

	DrawImageTransparent(pDC, pole1);

	Translate(pDC, 122, pole2->Height() / 2);

	Rotate(pDC, alpha);

	Translate(pDC, -32, -32);

	DrawImageTransparent(pDC, pole2);

	Translate(pDC, 140 + 32, 0 + 32);

	Rotate(pDC, 90 - beta);

	DrawBarrierBoom(pDC, s, nStrip, -beta + alpha);

	pDC->SetWorldTransform(&oldf);
}

CgdijbDoc* CgdijbView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CgdijbDoc)));
	return (CgdijbDoc*)m_pDocument;
}
#endif //_DEBUG


// CgdijbView message handlers


BOOL CgdijbView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
}


void CgdijbView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case 'A':
		ugaoRampa1 += 5;
		break;
	case 'S':
		ugaoRampa1 -= 5;
		break;
	case 'D':
		ugaoRampa2 += 5;
		break;
	case 'F':
		ugaoRampa2 -= 5;
		break;
	case 'Q':
		if (brojPrecki <= 16)
			brojPrecki++;
		break;
	case 'W':
		if (brojPrecki >= 2)
			brojPrecki--;
		break;
	case 'E':
		flagSvetlo = !flagSvetlo;
		if (flagSvetlo)
		{
			svetla[1] = svetla[0];
			svetla[0] = !svetla[1];
		}
		if (!flagSvetlo)
		{
			svetla[0] = false;
			svetla[1] = false;
		}
		break;
	case 'R':
		if (flagSvetlo)
		{
			svetla[1] = svetla[0];
			svetla[0] = !svetla[1];
		}
		break;
	default:
		break;
	}

	Invalidate();

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
