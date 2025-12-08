
// lab_zadatakView.cpp : implementation of the ClabzadatakView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "lab_zadatak.h"
#endif

#include "lab_zadatakDoc.h"
#include "lab_zadatakView.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define START_X 10
#define START_Y 10
#define KOCKICA 25
#define SIZE 501

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ClabzadatakView

IMPLEMENT_DYNCREATE(ClabzadatakView, CView)

BEGIN_MESSAGE_MAP(ClabzadatakView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// ClabzadatakView construction/destruction

ClabzadatakView::ClabzadatakView() noexcept
{
	// TODO: add construction code here

}

ClabzadatakView::~ClabzadatakView()
{
}

BOOL ClabzadatakView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// ClabzadatakView drawing

void ClabzadatakView::OnDraw(CDC* pDC)
{
	NacrtajSveOblike(pDC);

	if (this->mreza)
		NacrtajMrezu(pDC);

	SetFocus();
}


// ClabzadatakView printing

BOOL ClabzadatakView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void ClabzadatakView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void ClabzadatakView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// ClabzadatakView diagnostics

#ifdef _DEBUG
void ClabzadatakView::AssertValid() const
{
	CView::AssertValid();
}

void ClabzadatakView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

ClabzadatakDoc* ClabzadatakView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(ClabzadatakDoc)));
	return (ClabzadatakDoc*)m_pDocument;
}
#endif //_DEBUG


// ClabzadatakView message handlers

void ClabzadatakView::NacrtajSveOblike(CDC* pDC)
{
	CPen* oldPen = nullptr;
	CBrush* oldBrush = nullptr;

	this->SivaPozadina(pDC, oldPen, oldBrush);

	COLORREF cyan = RGB(0, 255, 255);

	// ljubicasti trougao
	POINT ljubicastiTrougao[3] = { Tacka(1, 12.75), Tacka(4.25, 9.5), Tacka(4.25, 16.25) };
	this->NacrtajOblik(pDC, ljubicastiTrougao, 3, cyan, RGB(173, 0, 230), -1, 6, 20, 0);

	// zuti kvadrat
	POINT zutiKvadrat[4] = { Tacka(4.25, 9.5), Tacka(4.25, 12.75), Tacka(7.5, 12.75), Tacka(7.5, 9.5) };
	this->NacrtajOblik(pDC, zutiKvadrat, 4, cyan, RGB(255, 255, 0), -1, 0, 0, 0);

	// crveni trougao
	POINT crveniTrougao[3] = { Tacka(7.5, 9.5), Tacka(7.5, 16.25), Tacka(14, 9.5)};
	this->NacrtajOblik(pDC, crveniTrougao, 3, cyan, RGB(255, 0, 0), -1, 6, 25, 0);

	// srafirani trougao
	POINT srafiraniTrougao[3] = { Tacka(7.5, 16.25), Tacka(10, 13.9), Tacka(10, 18.5) };
	this->NacrtajOblik(pDC, srafiraniTrougao, 3, cyan, RGB(0, 0, 255), HS_BDIAGONAL, 8, 15, 0);

	// zeleni trougao
	POINT zeleniTrougao[3] = { Tacka(7.5, 9.5), Tacka(17, 9.5), Tacka(12.25, 4.75) };
	this->NacrtajOblik(pDC, zeleniTrougao, 3, cyan, RGB(95, 189, 67), -1, 5, 25, 0);

	// roze paralelogram
	POINT rozeParalelogram[4] = { Tacka(12.25, 4.75), Tacka(15.5, 4.75), Tacka(19, 1.5), Tacka(15.5, 1.5) };
	this->NacrtajOblik(pDC, rozeParalelogram, 4, cyan, RGB(255, 105, 180), -1, 0, 0, 0);

	// narandzasti trougao
	POINT narandzastiTrougao[3] = { Tacka(15.5, 9.5), Tacka(19, 9.5), Tacka(19, 13) };
	this->NacrtajOblik(pDC, narandzastiTrougao, 3, cyan, RGB(255, 165, 0), -1, 8, 10, 0);
}

void ClabzadatakView::SivaPozadina(CDC* pDC, CPen*& oldPen, CBrush*& oldBrush)
{
	CPen newPen(PS_SOLID, 1, RGB(200, 200, 200));
	CBrush newBrush(RGB(230, 230, 230));
	SelektujOlovkuiCetkicu(pDC, newPen, newBrush, oldPen, oldBrush);

	pDC->Rectangle(START_X, START_Y, START_X + SIZE, START_Y + SIZE);

	VratiOlovkuiCetkicu(pDC, oldPen, oldBrush);
}

void ClabzadatakView::NacrtajMrezu(CDC* pDC)
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

POINT ClabzadatakView::Tacka(float brKockicaX, float brKockicaY)
{
	POINT X;
	X.x = START_X + (int)(KOCKICA * brKockicaX);
	X.y = START_Y + (int)(KOCKICA * brKockicaY);

	return X;
}

POINT ClabzadatakView::VratiCentarTrougla(POINT* temena)
{
	double a = this->Razdaljina(temena[1], temena[2]);
	double b = this->Razdaljina(temena[0], temena[2]);
	double c = this->Razdaljina(temena[0], temena[1]);

	POINT Centar = {};
	Centar.x = (temena[0].x * a + temena[1].x * b + temena[2].x * c) / (a + b + c);
	Centar.y = (temena[0].y * a + temena[1].y * b + temena[2].y * c) / (a + b + c);

	return Centar;
}

double ClabzadatakView::Razdaljina(POINT A, POINT B)
{
	return sqrt(pow(A.x - B.x, 2) + pow(A.y - B.y, 2));
}

void ClabzadatakView::SelektujOlovkuiCetkicu(CDC* pDC, CPen& pen, CBrush& brush, CPen*& oldPen, CBrush*& oldBrush)
{
	oldPen = pDC->SelectObject(&pen);
	oldBrush = pDC->SelectObject(&brush);
}

void ClabzadatakView::VratiOlovkuiCetkicu(CDC* pDC, CPen* oldPen, CBrush* oldBrush)
{
	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
}

void ClabzadatakView::NacrtajOblik(CDC* pDC, POINT* temena, int brTemena, COLORREF bojaOlovke, COLORREF bojaCetke, int tipSrafure, int polyN, int polyR, float polyRot)
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

		CBrush* tempBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
		POINT centar = VratiCentarTrougla(temena);
		DrawRegularPolygon(pDC, centar.x, centar.y, polyR, polyN, polyRot);

		pDC->SelectObject(tempBrush);
	}

	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
}

void ClabzadatakView::DrawRegularPolygon(CDC* pDC, int cx, int cy, int r, int n, float rotAngle)
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

void ClabzadatakView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == 'G')
		this->mreza = !this->mreza;

	Invalidate();

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void ClabzadatakView::OnLButtonDown(UINT nFlags, CPoint point)
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

	CView::OnLButtonDown(nFlags, point);
}
