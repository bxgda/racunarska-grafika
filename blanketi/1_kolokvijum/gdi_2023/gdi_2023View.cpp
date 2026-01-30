
// gdi_2023View.cpp : implementation of the Cgdi2023View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "gdi_2023.h"
#endif

#include "gdi_2023Doc.h"
#include "gdi_2023View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TO_RAD 3.14159265 / 180.0f
#define TO_DEG 180.0f / 3.14159265

// Cgdi2023View

IMPLEMENT_DYNCREATE(Cgdi2023View, CView)

BEGIN_MESSAGE_MAP(Cgdi2023View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// Cgdi2023View construction/destruction

Cgdi2023View::Cgdi2023View() noexcept
{
	glava = new DImage();
	glava->Load(CString("glava.png"));

	nadkolenica = new DImage();
	nadkolenica->Load(CString("nadkolenica.png"));
	
	nadlaktica = new DImage();
	nadlaktica->Load(CString("nadlaktica.png"));

	potkolenica = new DImage();
	potkolenica->Load(CString("podkolenica.png"));

	podlaktica = new DImage();
	podlaktica->Load(CString("podlaktica.png"));

	saka = new DImage();
	saka->Load(CString("saka.png"));

	stopalo = new DImage();
	stopalo->Load(CString("stopalo.png"));

	telo = new DImage();
	telo->Load(CString("telo.png"));

	pozadina = new DImage();
	pozadina->Load(CString("pozadina.jpg"));
}

Cgdi2023View::~Cgdi2023View()
{
	delete glava;
	delete nadkolenica;
	delete nadlaktica;
	delete potkolenica;
	delete podlaktica;
	delete saka;
	delete stopalo;
	delete telo;
	delete pozadina;
}

BOOL Cgdi2023View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// Cgdi2023View drawing

void Cgdi2023View::OnDraw(CDC* pDC)
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

	// kraj antiflicker-a --------------------------
	pDC->BitBlt(0, 0, pravougaonik.Width(), pravougaonik.Height(), pMemDC, 0, 0, SRCCOPY);
	pMemDC->DeleteDC();
	delete pMemDC;
	// ----------------------------------------
}


// Cgdi2023View printing

BOOL Cgdi2023View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void Cgdi2023View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void Cgdi2023View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// Cgdi2023View diagnostics

#ifdef _DEBUG
void Cgdi2023View::AssertValid() const
{
	CView::AssertValid();
}

void Cgdi2023View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

void Cgdi2023View::DrawCoordinateSystem(CDC* pDC)
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

void Cgdi2023View::DrawScene(CDC* pDC)
{
	DrawBackground(pDC);

	float pozadinaX = pozadina->Width() / 2.0f;
	float pozadinaY = pozadina->Height() / 2.0f;

	// skaliranje i rotiranje celog platna oko centra pozadina
	Translate(pDC, pozadinaX, pozadinaY);
	Rotate(pDC, ugaoCeoRobot * TO_RAD);
	Scale(pDC, skaliranjeRobota, skaliranjeRobota);
	Translate(pDC, -pozadinaX, -pozadinaY);

	// pozicija robota u centar pozadina
	Translate(pDC, pozadinaX - telo->Width(), pozadinaY - telo->Height());

	DrawRobot(pDC);
}

void Cgdi2023View::DrawBackground(CDC* pDC)
{
	CRect src(0, 0, pozadina->Width(), pozadina->Height());

	pozadina->Draw(pDC, src, src);
}

void Cgdi2023View::DrawImgTransparent(CDC* pDC, DImage* pImage)
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

void Cgdi2023View::Translate(CDC* pDC, float dX, float dY, bool rightMultiply)
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

void Cgdi2023View::Rotate(CDC* pDC, float angle, bool rightMultiply)
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

void Cgdi2023View::Scale(CDC* pDC, float sX, float sY, bool rightMultiply)
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

void Cgdi2023View::DrawHalf(CDC* pDC)
{
	DrawImgTransparent(pDC, telo);

	XFORM ruka{};
	pDC->GetWorldTransform(&ruka);

	// nadlaktica

	Translate(pDC, 25, 65);

	Rotate(pDC, ugaoNadlaktica * TO_RAD);
	Translate(pDC, -35, -35);

	DrawImgTransparent(pDC, nadlaktica);

	// podlaktica

	Translate(pDC, 22, 167);

	Rotate(pDC, ugaoPodlaktica * TO_RAD);
	Translate(pDC, -30, -33);

	DrawImgTransparent(pDC, podlaktica);

	// saka

	Translate(pDC, 30, 140);

	Rotate(pDC, ugaoSaka * TO_RAD);
	Translate(pDC, -25, -3);

	DrawImgTransparent(pDC, saka);

	pDC->SetWorldTransform(&ruka);

	// nadkolenica

	Translate(pDC, 61 - 29, 262 - 20);
	DrawImgTransparent(pDC, nadkolenica);

	// potkolenica

	Translate(pDC, 30 - 25, 184 - 37);
	DrawImgTransparent(pDC, potkolenica);

	// stopalo

	Translate(pDC, 25 - 20, 248 - 16);
	DrawImgTransparent(pDC, stopalo);	

	pDC->SetWorldTransform(&ruka);
}

void Cgdi2023View::DrawHead(CDC* pDC)
{
	DrawImgTransparent(pDC, glava);
}

void Cgdi2023View::DrawRobot(CDC* pDC)
{
	// leva strana
	DrawHalf(pDC);

	XFORM desnaStrana{};
	pDC->GetWorldTransform(&desnaStrana);

	// desna strana
	Translate(pDC, 2 * telo->Width(), 0);
	Scale(pDC, -1, 1); // ogledalo
	DrawHalf(pDC);

	pDC->SetWorldTransform(&desnaStrana);

	Translate(pDC, telo->Width() - glava->Width() / 2, -glava->Height() + 20);
	DrawHead(pDC);
}

Cgdi2023Doc* Cgdi2023View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cgdi2023Doc)));
	return (Cgdi2023Doc*)m_pDocument;
}
#endif //_DEBUG


// Cgdi2023View message handlers

BOOL Cgdi2023View::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
}

void Cgdi2023View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	float ugao = 5;

	switch (nChar)
	{
	case 'A':
		if (ugaoSaka <= 30)
			ugaoSaka += ugao;
		break;
	case 'S':
		if (ugaoSaka >= -10)
			ugaoSaka -= ugao;
		break;
	case 'D':
		if (ugaoPodlaktica <= 80)
			ugaoPodlaktica += ugao;
		break;
	case 'F':
		if (ugaoPodlaktica >= -10)
			ugaoPodlaktica -= ugao;
		break;
	case 'G':
		if (ugaoNadlaktica <= 90)
			ugaoNadlaktica += ugao;
		break;
	case 'H':
		if (ugaoNadlaktica >= -10)
			ugaoNadlaktica -= ugao;
		break;
	case '1':
		ugaoCeoRobot += ugao;
		skaliranjeRobota += 0.1f;
		break;
	case '2':
		ugaoCeoRobot -= ugao;
		skaliranjeRobota -= 0.1f;
		break;
	default:
		break;
	}
	Invalidate();

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
