
// vezba6View.cpp : implementation of the Cvezba6View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "vezba6.h"
#endif

#include "vezba6Doc.h"
#include "vezba6View.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define START_X 10
#define START_Y 10
#define KOCKICA 25
#define SIZE 501

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cvezba6View

IMPLEMENT_DYNCREATE(Cvezba6View, CView)

BEGIN_MESSAGE_MAP(Cvezba6View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// Cvezba6View construction/destruction

Cvezba6View::Cvezba6View() noexcept
{
	// TODO: add construction code here

}

Cvezba6View::~Cvezba6View()
{
}

BOOL Cvezba6View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// Cvezba6View drawing

void Cvezba6View::OnDraw(CDC* pDC)
{
	NacrtajSveOblike(pDC);

	if (this->mreza)
		NacrtajMrezu(pDC);

	SetFocus();
}


// Cvezba6View printing

BOOL Cvezba6View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void Cvezba6View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void Cvezba6View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// Cvezba6View diagnostics

#ifdef _DEBUG
void Cvezba6View::AssertValid() const
{
	CView::AssertValid();
}

void Cvezba6View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Cvezba6Doc* Cvezba6View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cvezba6Doc)));
	return (Cvezba6Doc*)m_pDocument;
}
#endif //_DEBUG


// Cvezba6View message handlers

void Cvezba6View::NacrtajSveOblike(CDC* pDC)
{
	CPen* oldPen = nullptr;
	CBrush* oldBrush = nullptr;

	this->SivaPozadina(pDC, oldPen, oldBrush);

	COLORREF zelena = RGB(0, 255, 0);

	// srafirani trougao
	POINT srafiraniTrougao[3] = { Tacka(1, 1), Tacka(1, 13), Tacka(13, 13) };
	this->NacrtajOblik(pDC, srafiraniTrougao, 3, zelena, RGB(0, 0, 255), HS_FDIAGONAL, 8, 40, 0);

	// zuti kvadrat
	POINT zutiKvadrat[4] = { Tacka(1, 13), Tacka(1, 19), Tacka(7, 19), Tacka(7, 13) };
	this->NacrtajOblik(pDC, zutiKvadrat, 4, zelena, RGB(255, 255, 0), -1, 0, 0, 0);

	// roze trougao
	POINT rozeTrougao[3] = { Tacka(1, 1), Tacka(13, 13), Tacka(13, 1) };
	this->NacrtajOblik(pDC, rozeTrougao, 3, zelena, RGB(255, 0, 255), -1, 5, 40, 0);

	// zeleni trougao
	POINT zeleniTrougao[3] = { Tacka(13, 1), Tacka(13, 7), Tacka(19, 1) };
	this->NacrtajOblik(pDC, zeleniTrougao, 3, zelena, RGB(0, 196, 2), -1, 4, 20, 0);

	// ljubicasti paralelogram
	POINT ljubicastiParalelogram[4] = { Tacka(13, 7), Tacka(13, 13), Tacka(19, 7), Tacka(19, 1) };
	this->NacrtajOblik(pDC, ljubicastiParalelogram, 4, zelena, RGB(128, 0, 128), -1, 0, 0, 0);

	// crveni trougao
	POINT crveniTrougao[3] = { Tacka(19, 7), Tacka(13, 13), Tacka(19, 19) };
	this->NacrtajOblik(pDC, crveniTrougao, 3, zelena, RGB(255, 0, 0), -1, 6, 30, 0);

	// narandzasti trougao
	POINT narandzastiTrougao[3] = { Tacka(13, 13), Tacka(19, 19), Tacka(13, 19) };
	this->NacrtajOblik(pDC, narandzastiTrougao, 3, zelena, RGB(255, 165, 0), -1, 7, 20, 52);
}

void Cvezba6View::SivaPozadina(CDC* pDC, CPen*& oldPen, CBrush*& oldBrush)
{
	CPen newPen(PS_SOLID, 1, RGB(200, 200, 200));
	CBrush newBrush(RGB(230, 230, 230));
	SelektujOlovkuiCetkicu(pDC, newPen, newBrush, oldPen, oldBrush);

	pDC->Rectangle(START_X, START_Y, START_X + SIZE, START_Y + SIZE);

	VratiOlovkuiCetkicu(pDC, oldPen, oldBrush);
}

void Cvezba6View::NacrtajMrezu(CDC* pDC)
{
	CPen SiviGrid(PS_SOLID, 1, RGB(250, 250, 250));
	CPen* oldPen = pDC->SelectObject(&SiviGrid);

	for (int i = START_X; i < START_X + SIZE; i += KOCKICA) {
		pDC->MoveTo(i, START_Y);
		pDC->LineTo(i, START_Y + SIZE);

		pDC->MoveTo(START_Y, i);
		pDC->LineTo(START_Y + SIZE, i);
	}

	pDC->SelectObject(oldPen);
}

POINT Cvezba6View::Tacka(float brKockicaX, float brKockicaY)
{
	POINT X;
	X.x = START_X + (int)(KOCKICA * brKockicaX);
	X.y = START_Y + (int)(KOCKICA * brKockicaY);

	return X;
}

POINT Cvezba6View::VratiCentarTrougla(POINT* temena)
{
	double a = this->Razdaljina(temena[1], temena[2]);
	double b = this->Razdaljina(temena[0], temena[2]);
	double c = this->Razdaljina(temena[0], temena[1]);

	POINT Centar = {};
	Centar.x = (temena[0].x * a + temena[1].x * b + temena[2].x * c) / (a + b + c);
	Centar.y = (temena[0].y * a + temena[1].y * b + temena[2].y * c) / (a + b + c);

	return Centar;
}

double Cvezba6View::Razdaljina(POINT A, POINT B)
{
	return sqrt(pow(A.x - B.x, 2) + pow(A.y - B.y, 2));
}

void Cvezba6View::SelektujOlovkuiCetkicu(CDC* pDC, CPen& pen, CBrush& brush, CPen*& oldPen, CBrush*& oldBrush)
{
	oldPen = pDC->SelectObject(&pen);
	oldBrush = pDC->SelectObject(&brush);
}

void Cvezba6View::VratiOlovkuiCetkicu(CDC* pDC, CPen* oldPen, CBrush* oldBrush)
{
	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
}

void Cvezba6View::NacrtajOblik(CDC* pDC, POINT* temena, int brTemena, COLORREF bojaOlovke, COLORREF bojaCetke, int tipSrafure, int polyN, int polyR, float polyRot)
{
	CPen novaOlovka(PS_SOLID, 6, bojaOlovke);

	CBrush novaCetka;
	if (tipSrafure == -1) 
		novaCetka.CreateSolidBrush(bojaCetke);
	else 
		novaCetka.CreateHatchBrush(tipSrafure, bojaCetke);

	CPen* oldPen = pDC->SelectObject(&novaOlovka);
	CBrush* oldBrush = pDC->SelectObject(&novaCetka);

	pDC->Polygon(temena, brTemena);

	if (polyN > 0 && brTemena == 3)
	{
		CPen tanjaOlovka(PS_SOLID, 2, bojaOlovke);
		pDC->SelectObject(&tanjaOlovka);

		POINT centar = VratiCentarTrougla(temena);
		DrawRegularPolygon(pDC, centar.x, centar.y, polyR, polyN, polyRot);
	}

	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
}

void Cvezba6View::DrawRegularPolygon(CDC* pDC, int cx, int cy, int r, int n, float rotAngle)
{
	POINT* Points = new POINT[n];

	float alfa = rotAngle; 

	float factor = 2 * M_PI / (double)n; 

	for (int i = 0; i < n; i++) {
		Points[i].x = (int)(cx + r * cos(alfa));
		Points[i].y = (int)(cy + r * sin(alfa));

		alfa += factor;
	}

	pDC->Polygon(Points, n);

	delete[] Points;
}

void Cvezba6View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == 'G')
		this->mreza = !this->mreza;

	Invalidate();

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void Cvezba6View::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (this->mreza)
	{
		int x = point.x;
		int y = point.y;

		if (x >= START_X && x <= START_X + SIZE &&
			y >= START_Y && y <= START_Y + SIZE)
		{

			double rawX = (double)(x - START_X) / KOCKICA;
			double rawY = (double)(y - START_Y) / KOCKICA;

			double kolona = floor(rawX * 2 + 0.5) / 2.0;
			double vrsta = floor(rawY * 2 + 0.5) / 2.0;

			CString msg;
			msg.Format(L"(%.1f, %.1f)", kolona, vrsta);
			AfxMessageBox(msg);
		}
	}

	CView::OnLButtonDown(nFlags, point);
}
