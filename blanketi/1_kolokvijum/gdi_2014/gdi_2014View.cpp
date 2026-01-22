
// gdi_2014View.cpp : implementation of the Cgdi2014View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "gdi_2014.h"
#endif

#include "gdi_2014Doc.h"
#include "gdi_2014View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TO_RAD 3.14159265 / 180.0f
#define TO_DEG 180.0f / 3.14159265


// Cgdi2014View

IMPLEMENT_DYNCREATE(Cgdi2014View, CView)

BEGIN_MESSAGE_MAP(Cgdi2014View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// Cgdi2014View construction/destruction

Cgdi2014View::Cgdi2014View() noexcept
{
	// konstruktor - ucitavamo slike i metafajlove

	pozicijaAuta = 0.0f;
	ugaoNagiba = 10.0f; // stepeni

	tocak = new DImage();
	tocak->Load(CString("Wheel.png"));

	reno = GetEnhMetaFile(CString("clio.emf"));
}

Cgdi2014View::~Cgdi2014View()
{
	// destruktor - oslobadjamo memoriju

	if (tocak)
		delete tocak;

	if (reno)
		DeleteEnhMetaFile(reno);
}

BOOL Cgdi2014View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// Cgdi2014View drawing

void Cgdi2014View::OnDraw(CDC* pDC)
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

	// plavo nebo
	CBrush cekta(RGB(200, 220, 255));
	CBrush* staraCekta = pMemDC->SelectObject(&cekta);

	pMemDC->Rectangle(0, 0, pravougaonik.Width(), pravougaonik.Height());
	pMemDC->SelectObject(staraCekta);

	XFORM staraTransformacija;
	int gm = pMemDC->SetGraphicsMode(GM_ADVANCED);
	pMemDC->GetWorldTransform(&staraTransformacija);

	DrawGround(pMemDC, ugaoNagiba);

	int sirinaAuta = 450;
	int visinaAuta = sirinaAuta / 2.5;

	// formula kako da vidimo koliko je auto rotirao tockove
	float ugaoTocka = TO_DEG * pozicijaAuta / (2.0f * 3.14159265f);

	pMemDC->GetWorldTransform(&staraTransformacija);

	// desno mnozenje
	Translate(pMemDC, 0.0, pravougaonik.Height(), false);
	Rotate(pMemDC, -ugaoNagiba * TO_RAD, false);
	Translate(pMemDC, sirinaAuta / 2, -visinaAuta / 2 - 38 / 2, false);
	
	DrawCar(pMemDC, pozicijaAuta, 0, sirinaAuta, visinaAuta);

	DrawWheel(pMemDC, pozicijaAuta - 155, 70, 38, ugaoTocka);
	DrawWheel(pMemDC, pozicijaAuta + 135, 70, 38, ugaoTocka);

	pMemDC->SetWorldTransform(&staraTransformacija);
	pMemDC->SetGraphicsMode(gm);

	// kraj antiflicker-a
	pDC->BitBlt(0, 0, pravougaonik.Width(), pravougaonik.Height(), pMemDC, 0, 0, SRCCOPY);
	pMemDC->DeleteDC();
	delete pMemDC;
}


// Cgdi2014View printing

BOOL Cgdi2014View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void Cgdi2014View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void Cgdi2014View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// Cgdi2014View diagnostics

#ifdef _DEBUG
void Cgdi2014View::AssertValid() const
{
	CView::AssertValid();
}

void Cgdi2014View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

void Cgdi2014View::Scale(CDC* pDC, float sX, float sY, bool rightMultiply)
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

void Cgdi2014View::Rotate(CDC* pDC, float angle, bool rightMultiply)
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

void Cgdi2014View::Translate(CDC* pDC, float dX, float dY, bool rightMultiply)
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

void Cgdi2014View::DrawGround(CDC* pDC, float angle)
{
	// crtamo podlogu pod uglom 'angle'

	CRect rect;
	GetClientRect(&rect);

	CPen olovka(PS_SOLID, 3, RGB(128, 32, 32));
	CPen* staraOlovka = pDC->SelectObject(&olovka);

	CBrush cetka(HS_FDIAGONAL, RGB(128, 32, 32));
	CBrush* staraCetka = pDC->SelectObject(&cetka);

	COLORREF staraBoja = pDC->SetBkColor(RGB(220, 255, 192));

	POINT tacke[3];

	tacke[0].x = rect.left;
	tacke[0].y = rect.bottom;

	tacke[1].x = rect.right;
	tacke[1].y = rect.bottom;

	// tg(angle) = visina / sirina
	// visina = sirina * tg(angle)
	// posto je y koordinata od gore prema dole, oduzimamo b starnicu visina od cele visine prozora da bi dobili y koordinatu

	tacke[2].x = rect.right;
	tacke[2].y = rect.bottom - rect.Width() * tan(angle * TO_RAD);

	pDC->Polygon(tacke, 3);

	pDC->SetBkColor(staraBoja);
	pDC->SelectObject(staraCetka);
	pDC->SelectObject(staraOlovka);
}

void Cgdi2014View::DrawCar(CDC* pDC, int x, int y, int w, int h)
{
	// iscrtavamo kontra nego sto je u uriginalu slika, moze i sa transformacijama ali ovako je lakse

	pDC->PlayMetaFile(reno, CRect(x + w / 2, y - h / 2, x - w / 2, y + h / 2));
}

void Cgdi2014View::DrawWheel(CDC* pDC, int x, int y, int r, float angle)
{
	// iscrtavamo tocak centrirano u (x, y) sa poluprecnikom r i pod uglom angle
	// sacuvamo staru transformaciju
	XFORM staraTransformacija;
	pDC->GetWorldTransform(&staraTransformacija);

	// i ovde mora desno mnozenje jer smo tako i u OnDraw uradili

	// pomerimo koordinatni sistem u centar tocka
	Translate(pDC, (float)x, (float)y, false);

	// rotiramo za dati ugao
	Rotate(pDC, angle * TO_RAD, false);
	
	// dato u tekstu zadatka
	int dx = 52, dy = 15, w = 376;

	CRect izvorniKvadrat(dx, dy, dx + w, dy + w);
	CRect izlazniKvadrat(-r, -r, r, r);
	// ovo nema u DImage koji se nalazi na cs-u pa sam dodao sam 
	tocak->DrawTransparent(pDC, izvorniKvadrat, izlazniKvadrat, RGB(255, 255, 255));

	pDC->SetWorldTransform(&staraTransformacija);
}

Cgdi2014Doc* Cgdi2014View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cgdi2014Doc)));
	return (Cgdi2014Doc*)m_pDocument;
}
#endif //_DEBUG


// Cgdi2014View message handlers

void Cgdi2014View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_LEFT)
	{
		pozicijaAuta -= 10.0f;
		if (pozicijaAuta < 0.0f)
			pozicijaAuta = 0.0f;
		Invalidate();
	}
	if (nChar == VK_RIGHT)
	{
		pozicijaAuta += 10.0f;
		Invalidate();
	}
	if (nChar == VK_UP)
	{
		pozicijaAuta = 0.0f;
		ugaoNagiba += 1.0f;
		if (ugaoNagiba > 80.0f)
			ugaoNagiba = 80.0f;
		Invalidate();
	}
	if (nChar == VK_DOWN)
	{
		pozicijaAuta = 0.0f;
		ugaoNagiba -= 1.0f;
		if (ugaoNagiba < -10.0f)
			ugaoNagiba = -10.0f;
		Invalidate();
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
