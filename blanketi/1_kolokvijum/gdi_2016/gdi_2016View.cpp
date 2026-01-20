
// gdi_2016View.cpp : implementation of the Cgdi2016View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "gdi_2016.h"
#endif

#include "gdi_2016Doc.h"
#include "gdi_2016View.h"

#define TO_RAD 3.14159265 / 180.0f
#define TO_DEG 180.0f / 3.14159265

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cgdi2016View

IMPLEMENT_DYNCREATE(Cgdi2016View, CView)

BEGIN_MESSAGE_MAP(Cgdi2016View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// Cgdi2016View construction/destruction

Cgdi2016View::Cgdi2016View() noexcept
{
	ugaoStapa = 0.0f;
	pomerajStapa = 20.0f;
	pomerajKugle = 0.0f;
	ugaoUdarca = 0.0f;

	zelenaPodloga = new DImage();
	drveniOkvir = new DImage();

	zelenaPodloga->Load(CString(L"felt2.jpg"));
	drveniOkvir->Load(CString(L"wood.jpg"));
}

Cgdi2016View::~Cgdi2016View()
{
}

BOOL Cgdi2016View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// Cgdi2016View drawing

void Cgdi2016View::OnDraw(CDC* pDC)
{

	// antiflicker --------------------------------------------------
	CRect pravougaonik;
	GetClientRect(&pravougaonik);

	CDC* pMemDC = new CDC();
	pMemDC->CreateCompatibleDC(pDC);
	CBitmap bpm;
	bpm.CreateCompatibleBitmap(pDC, pravougaonik.Width(), pravougaonik.Height());
	pMemDC->SelectObject(&bpm);
	// --------------------------------------------------------------

	XFORM staraTransformacija;
	int gm = pMemDC->SetGraphicsMode(GM_ADVANCED);
	pMemDC->GetWorldTransform(&staraTransformacija);

	DrawTable(pMemDC, pravougaonik);
	DrawBorder(pMemDC, pravougaonik, 30);
	DrawHoles(pMemDC, pravougaonik, 30);

	// jebem ti ja ovu fiziku za stap i kuglu

	// dolazimo u tacku koja je odredjena zadatkom
	Translate(pMemDC, 600, 400, false);

	// pamtimo ovu transformaciju kao centar za kuglu gde se ona vraca
	XFORM centarTransformacija;
	pMemDC->GetWorldTransform(&centarTransformacija);

	// crtamo kuglu
	if (pomerajKugle > 0)
	{
		Rotate(pMemDC, ugaoUdarca, false); // koristi fiksni ugao udarca
		Translate(pMemDC, 0, -pomerajKugle, false);
		DrawBall(pMemDC, 30);
	}
	else
	{
		DrawBall(pMemDC, 30); // Kugla je u centru
	}

	// vracamo se u centar odredjen zadatkom
	pMemDC->SetWorldTransform(&centarTransformacija);

	// crtamo stap
	Rotate(pMemDC, ugaoStapa, false); // Štap se uvijek rotira po komandi korisnika

	if (pomerajKugle > 0)
	{
		// kugla je u pokretu a stap ceka gde je bio - fiksna pozicija stapa
		Translate(pMemDC, 0, 15.0f, false);
	}
	else
	{
		// kugla je tu a stap se rotira oko nje - pomera se zajedno sa kuglom
		Translate(pMemDC, 0, pomerajStapa, false);
	}

	DrawStick(pMemDC, 600);

	pMemDC->SetWorldTransform(&staraTransformacija);
	pMemDC->SetGraphicsMode(gm);

	// kraj antiflicker-a
	pDC->BitBlt(0, 0, pravougaonik.Width(), pravougaonik.Height(), pMemDC, 0, 0, SRCCOPY);
	pMemDC->DeleteDC();
	delete pMemDC;
}


// Cgdi2016View printing

BOOL Cgdi2016View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void Cgdi2016View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void Cgdi2016View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// Cgdi2016View diagnostics

#ifdef _DEBUG
void Cgdi2016View::AssertValid() const
{
	CView::AssertValid();
}

void Cgdi2016View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

void Cgdi2016View::DrawStick(CDC* pDC, int w)
{
	// crtamo bilijarski stap duzine w, sirine w/100 na vrhu i dvostruko vece sirine na dnu
	// 2/3 stapa obojeno je oker bojom a 1/3 braon

	int gornjaSirina = w / 100;
	int donjaSirina = 2 * gornjaSirina;

	int polovinaGornjeSirine = gornjaSirina / 2;
	int polovinaSrednjeSirine = (2 * gornjaSirina) / 3;
	int polovinaDonjeSirine = donjaSirina / 2;

	int yGore = 0;
	int yDole = w;

	int xCentar = 0;

	int y2_3_split = (2 * w) / 3;

	const COLORREF braon = RGB(139, 69, 19);
	const COLORREF oker = RGB(204, 153, 51);

	CPen spoljnaOlovka(PS_SOLID, 1, braon);
	CPen* oldPen = pDC->SelectObject(&spoljnaOlovka);

	CBrush okerCetka(oker);
	CBrush braonCetka(braon);

	CBrush* oldBrush = pDC->SelectObject(&okerCetka);

	POINT gornjeTacke[4] = {
		{ xCentar - polovinaGornjeSirine, yGore },
		{ xCentar + polovinaGornjeSirine, yGore },
		{ xCentar + polovinaSrednjeSirine, y2_3_split },
		{ xCentar - polovinaSrednjeSirine, y2_3_split }
	};

	pDC->Polygon(gornjeTacke, 4);

	pDC->SelectObject(&braonCetka);

	POINT donjeTacke[4] = {
		{ xCentar - polovinaSrednjeSirine, y2_3_split },
		{ xCentar + polovinaSrednjeSirine, y2_3_split },
		{ xCentar + polovinaDonjeSirine, yDole },
		{ xCentar - polovinaDonjeSirine, yDole }
	};

	pDC->Polygon(donjeTacke, 4);

	pDC->Ellipse(
		xCentar - polovinaDonjeSirine,
		yDole - polovinaDonjeSirine,
		xCentar + polovinaDonjeSirine,
		yDole + polovinaDonjeSirine
	);

	CPen odsjajOlovka(PS_SOLID, 1, RGB(255, 255, 255));

	pDC->SelectObject(&odsjajOlovka);

	pDC->MoveTo(xCentar + polovinaGornjeSirine - 2, yGore + 2);
	pDC->LineTo(xCentar + polovinaDonjeSirine - 2, yDole - 2);

	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
}

void Cgdi2016View::DrawBall(CDC* pDC, int w)
{
	int r = w / 2;

	COLORREF tamnoCrvena = RGB(139, 0, 0);
	COLORREF josTamnijeCrvena = RGB(90, 0, 0);

	CPen okvirOlovka(PS_SOLID, 1, josTamnijeCrvena);
	CPen* oldPen = pDC->SelectObject(&okvirOlovka);

	CBrush crvenaCetka(tamnoCrvena);
	CBrush* oldBrush = pDC->SelectObject(&crvenaCetka);

	pDC->Ellipse(-r, -r, r, r);

	CPen odsjajOlovka(PS_SOLID, 1, RGB(255, 255, 255));
	pDC->SelectObject(&odsjajOlovka);

	CBrush odsjajCetka(RGB(255, 255, 255));
	pDC->SelectObject(odsjajCetka);

	pDC->Ellipse(0, 0, r / 2, r / 2);

	pDC->SelectObject(oldBrush);
	pDC->SelectObject(oldPen);
}

void Cgdi2016View::Translate(CDC* pDC, float dX, float dY, bool rightMultiply)
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

void Cgdi2016View::Rotate(CDC* pDC, float angle, bool rightMultiply)
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

void Cgdi2016View::DrawTable(CDC* pDC, const CRect& rect)
{
	const int tileW = zelenaPodloga->Width();
	const int tileH = zelenaPodloga->Height();
	if (tileW <= 0 || tileH <= 0)
		return;

	// iscrtavamo tile po tile dok ne popunimo ceo pravougaonik
	for (int y = rect.top; y < rect.bottom; y += tileH)
	{
		for (int x = rect.left; x < rect.right; x += tileW)
		{
			const int preostalaSirina = rect.right - x;
			const int preostalaVisina = rect.bottom - y;

			int drawSirina;
			if (preostalaSirina < tileW)
				drawSirina = preostalaSirina;
			else
				drawSirina = tileW;

			int drawVisina;
			if (preostalaVisina < tileH)
				drawVisina = preostalaVisina;
			else
				drawVisina = tileH;

			// src: deo slike koji uzimamo (odsecamo ako treba)
			CRect src(0, 0, drawSirina, drawVisina);

			// dst: deo povrsine stola koji popunjavamo (isti drawW/drawH => nema skaliranja)
			CRect dst(x, y, x + drawSirina, y + drawVisina);

			zelenaPodloga->Draw(pDC, src, dst);
		}
	}
}

void Cgdi2016View::DrawBorder(CDC* pDC, CRect rect, int w)
{
	const int imgW = drveniOkvir->Width();
	const int imgH = drveniOkvir->Height();

	// uzimamo celu sliku kao source
	CRect src(0, 0, imgW, imgH);

	// trake (dest pravougaonici)
	CRect top(rect.left, rect.top, rect.right, rect.top + w);
	CRect bottom(rect.left, rect.bottom - w, rect.right, rect.bottom);
	CRect left(rect.left, rect.top + w, rect.left + w, rect.bottom - w);
	CRect right(rect.right - w, rect.top + w, rect.right, rect.bottom - w);

	// gore / dole
	drveniOkvir->Draw(pDC, src, top);
	drveniOkvir->Draw(pDC, src, bottom);

	// levo / desno
	drveniOkvir->Draw(pDC, src, left);
	drveniOkvir->Draw(pDC, src, right);
}

void Cgdi2016View::DrawHoles(CDC* pDC, CRect rect, int size)
{
	const int r = size / 2;

	const int cxL = rect.left + size;
	const int cxC = rect.left + rect.Width() / 2;
	const int cxR = rect.right - size;

	const int cyT = rect.top + size;
	const int cyB = rect.bottom - size;

	CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen* oldPen = pDC->SelectObject(&pen);

	CBrush brush(RGB(0, 0, 0));
	CBrush* oldBrush = pDC->SelectObject(&brush);

	// gore
	pDC->Ellipse(cxL - r, cyT - r, cxL + r, cyT + r);
	pDC->Ellipse(cxC - r, cyT - r, cxC + r, cyT + r);
	pDC->Ellipse(cxR - r, cyT - r, cxR + r, cyT + r);

	// dole
	pDC->Ellipse(cxL - r, cyB - r, cxL + r, cyB + r);
	pDC->Ellipse(cxC - r, cyB - r, cxC + r, cyB + r);
	pDC->Ellipse(cxR - r, cyB - r, cxR + r, cyB + r);

	pDC->SelectObject(oldBrush);
	pDC->SelectObject(oldPen);
}

Cgdi2016Doc* Cgdi2016View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cgdi2016Doc)));
	return (Cgdi2016Doc*)m_pDocument;
}
#endif //_DEBUG


// Cgdi2016View message handlers

void Cgdi2016View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	float granicaUdara = 15.0f; // Tačka kontakta (poluprečnik kugle)

	if (nChar == VK_LEFT)
	{
		ugaoStapa -= 5.0f * TO_RAD;
		Invalidate();
	}
	if (nChar == VK_RIGHT)
	{
		ugaoStapa += 5.0f * TO_RAD;
		Invalidate();
	}
	if (nChar == VK_UP)
	{
		if (pomerajKugle > 0)
		{
			// kugla je vec udarena i putuje
			pomerajKugle += 5.0f;
		}
		else if (pomerajStapa <= granicaUdara)
		{
			// stap je stigao do kugle i sad nastavlja kugla dalje
			if (pomerajKugle == 0) ugaoUdarca = ugaoStapa; // fiksiramo pravac kretanja kugle
			pomerajKugle += 5.0f;
			pomerajStapa = granicaUdara; // stap ostaje tu gde je bio
		}
		else
		{
			// stap se jos uvek priblizava kugli
			pomerajStapa -= 5.0f;
		}
		Invalidate();
	}
	if (nChar == VK_DOWN)
	{
		if (pomerajKugle > 0)
		{
			// prvo vracamo kuglu prema stapu
			pomerajKugle -= 5.0f;
			if (pomerajKugle < 0) pomerajKugle = 0;
		}
		else
		{
			// tek kada se kugla vratila stap natavlja da se pomera unazad
			pomerajStapa += 5.0f;
		}
		Invalidate();
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
