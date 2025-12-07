
// vezba1View.cpp : implementation of the Cvezba1View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "vezba1.h"
#endif

#include "vezba1Doc.h"
#include "vezba1View.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define START_X 10
#define START_Y 10
#define KOCKICA 25
#define SIZE 501

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cvezba1View

IMPLEMENT_DYNCREATE(Cvezba1View, CView)

BEGIN_MESSAGE_MAP(Cvezba1View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// Cvezba1View construction/destruction

Cvezba1View::Cvezba1View() noexcept
{
	this->mreza = true;
}

Cvezba1View::~Cvezba1View()
{
}

BOOL Cvezba1View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// Cvezba1View drawing

void Cvezba1View::OnDraw(CDC* pDC)
{
	NacrtajSveOblike(pDC);

	if (this->mreza)
		NacrtajMrezu(pDC);
	
	SetFocus();
}


// Cvezba1View printing

BOOL Cvezba1View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void Cvezba1View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void Cvezba1View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// Cvezba1View diagnostics

#ifdef _DEBUG
void Cvezba1View::AssertValid() const
{
	CView::AssertValid();
}

void Cvezba1View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

void Cvezba1View::SivaPozadina(CDC* pDC, CPen*& oldPen, CBrush*& oldBrush)
{
	CPen newPen(PS_SOLID, 1, RGB(200, 200, 200));
	CBrush newBrush(RGB(230, 230, 230));

	SelektujOlovkuiCetkicu(pDC, newPen, newBrush, oldPen, oldBrush);

	pDC->Rectangle(START_X, START_Y, START_X + SIZE, START_Y + SIZE);

	VratiOlovkuiCetkicu(pDC, oldPen, oldBrush);
}

void Cvezba1View::NacrtajSveOblike(CDC* pDC)
{
	CPen* oldPen = nullptr;
	CBrush* oldBrush = nullptr;
	
	SivaPozadina(pDC, oldPen, oldBrush);
	
	COLORREF cijan = RGB(0, 255, 255);
	
	// --- 1. Gornji trougao 
	POINT t1[] = { Tacka(10, 5.5), Tacka(7, 8.5), Tacka(13, 8.5) };
	NacrtajOblik(pDC, t1, 3, cijan, RGB(255, 0, 0), -1, 5, 15, 10);
	
	
	// --- 2. Levi trougao 
	POINT t2[] = { Tacka(1, 14.5), Tacka(7, 14.5), Tacka(7, 8.5) };
	NacrtajOblik(pDC, t2, 3, cijan, RGB(10, 199, 0), -1, 7, 20, 10);
	
	
	// --- 3. Narandzasti kvadar 
	POINT t3[] = { Tacka(7, 8.5), Tacka(10, 11.5), Tacka(10, 14.5), Tacka(7, 11.5) };
	NacrtajOblik(pDC, t3, 4, cijan, RGB(255, 165, 0), -1, 0, 0, 0);
	
	
	// --- 4. Srafirani trougao 
	POINT t4[] = { Tacka(7, 11.5), Tacka(7, 14.5), Tacka(10, 14.5) };
	NacrtajOblik(pDC, t4, 3, cijan, RGB(0, 0, 255), HS_HORIZONTAL, 8, 10, 10);
	
	
	// --- 5. Ljubicasti trougao
	POINT t5[] = { Tacka(10, 11.5), Tacka(13, 8.5), Tacka(13, 11.5) };
	NacrtajOblik(pDC, t5, 3, cijan, RGB(128, 0, 128), -1, 6, 10, 0);
	
	
	// --- 6. Zuti trougao 
	POINT t6[] = { Tacka(13, 8.5), Tacka(13, 14.5), Tacka(19, 14.5) };
	NacrtajOblik(pDC, t6, 3, cijan, RGB(255, 255, 0), -1, 4, 25, 0);
	
	
	// --- 7. Roze kvadrat 
	// Njega i dalje crtamo rucno jer je rotiran i koristi DrawRegularPolygon za spoljni oblik
	{
		CPen olovka(PS_SOLID, 4, cijan);
		CBrush cetka(RGB(255, 105, 180));
		SelektujOlovkuiCetkicu(pDC, olovka, cetka, oldPen, oldBrush);
	
		POINT ck = Tacka(11.5, 13);
		DrawRegularPolygon(pDC, ck.x, ck.y, 1.5 * KOCKICA * sqrt(2), 4, M_PI_4);
	
		VratiOlovkuiCetkicu(pDC, oldPen, oldBrush);
	}

}

void Cvezba1View::NacrtajMrezu(CDC* pDC)
{
	CPen SiviGrid(PS_SOLID, 1, RGB(250, 250, 250));

	CPen* oldPen = pDC->SelectObject(&SiviGrid);

	for (int i = START_X; i < START_X + SIZE; i += KOCKICA) {
		// crtanje vertikalnih linija
		pDC->MoveTo(i, START_Y);
		pDC->LineTo(i, START_Y + SIZE);

		// crtanje horizontalnih linija
		pDC->MoveTo(START_Y, i);
		pDC->LineTo(START_Y + SIZE, i);
	}

	pDC->SelectObject(oldPen);
}

POINT Cvezba1View::Tacka(float brKockicaX, float brKockicaY)
{
	POINT X;
	X.x = START_X + (int)(KOCKICA * brKockicaX);
	X.y = START_Y + (int)(KOCKICA * brKockicaY);

	return X;
}

POINT Cvezba1View::VratiCentarTrougla(POINT* temena)
{
	double a = this->Razdaljina(temena[1], temena[2]);
	double b = this->Razdaljina(temena[0], temena[2]);
	double c = this->Razdaljina(temena[0], temena[1]);

	POINT Centar = {};
	Centar.x = (temena[0].x * a + temena[1].x * b + temena[2].x * c) / (a + b + c);
	Centar.y = (temena[0].y * a + temena[1].y * b + temena[2].y * c) / (a + b + c);

	return Centar;
}

double Cvezba1View::Razdaljina(POINT A, POINT B)
{
	return sqrt(pow(A.x - B.x, 2) + pow(A.y - B.y, 2));
}

void Cvezba1View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == 'G')
		this->mreza = !this->mreza;

	Invalidate();

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void Cvezba1View::SelektujOlovkuiCetkicu(CDC* pDC, CPen& pen, CBrush& brush, CPen*& oldPen, CBrush*& oldBrush)
{
	oldPen = pDC->SelectObject(&pen);
	oldBrush = pDC->SelectObject(&brush);
}

void Cvezba1View::VratiOlovkuiCetkicu(CDC* pDC, CPen* oldPen, CBrush* oldBrush)
{
	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
}

void Cvezba1View::NacrtajOblik(CDC* pDC, POINT* temena, int brTemena, COLORREF bojaOlovke, COLORREF bojaCetke, int tipSrafure, int polyN, int polyR, float polyRot)
{
	// -----------------------------------------------------------------------------
	// Univerzalna funkcija za crtanje spoljasnjeg oblika (trougao, cetvorougao...)
	// sa opcionim iscrtavanjem pravilnog poligona u njegovom centru.
	// 
	// Parametri:
	//   pDC        -> Pokazivac na kontekst uredjaja (gde crtamo).
	//   temena     -> Niz tacaka (POINT) koje definisu temena spoljasnjeg oblika.
	//   brTemena   -> Broj tacaka u nizu (npr. 3 za trougao, 4 za cetvorougao).
	//   bojaOlovke -> Boja kojom se iscrtavaju ivice (RGB).
	//   bojaCetke  -> Boja kojom se ispunjava oblik (RGB).
	//   tipSrafure -> Odredjuje tip ispune:
	//                 * -1            = Puna boja (Solid Brush)
	//                 * HS_HORIZONTAL = Horizontalne linije
	//                 * HS_VERTICAL   = Vertikalne linije (itd...)
	//   polyN      -> Broj stranica UNUTRASNJEG poligona:
	//                 * 0   = Ne iscrtava se unutrasnji oblik.
	//                 * > 2 = Iscrtava se pravilni poligon (npr. 5 za petougao).
	//   polyR      -> Poluprecnik unutrasnjeg poligona (ako se crta).
	//   polyRot    -> Ugao rotacije unutrasnjeg poligona (u radijanima).
	// -----------------------------------------------------------------------------

	// 1. Kreiranje olovke (koristimo prosledjenu boju)
	CPen novaOlovka(PS_SOLID, 4, bojaOlovke);

	// 2. Kreiranje četke (Solid ili Hatch zavisno od parametra)
	CBrush novaCetka;
	if (tipSrafure == -1) {
		// Ako je -1, crtamo punu boju
		novaCetka.CreateSolidBrush(bojaCetke);
	}
	else {
		// U suprotnom pravimo srafuru (HS_HORIZONTAL, HS_VERTICAL, itd.)
		novaCetka.CreateHatchBrush(tipSrafure, bojaCetke);

		// *Napomena: Kod srafure, pozadina ostaje providna ili bela, 
		// zavisno od SetBkMode. Ovde podrazumevamo default ponasanje.*
	}

	// 3. Selektovanje
	CPen* oldPen = pDC->SelectObject(&novaOlovka);
	CBrush* oldBrush = pDC->SelectObject(&novaCetka);

	// 4. Crtanje spoljasnjeg oblika
	pDC->Polygon(temena, brTemena);

	// 5. Crtanje unutrašnjeg poligona (samo ako postoji polyN > 0)
	if (polyN > 0 && brTemena == 3)
	{
		// Unutrasnji oblik crtamo istom bojom olovke, ali tanjom (debljina 2)
		CPen tanjaOlovka(PS_SOLID, 2, bojaOlovke);
		pDC->SelectObject(&tanjaOlovka);

		POINT centar = VratiCentarTrougla(temena);
		DrawRegularPolygon(pDC, centar.x, centar.y, polyR, polyN, polyRot);
	}

	// 6. Cleanup
	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
}

void Cvezba1View::DrawRegularPolygon(CDC* pDC, int cx, int cy, int r, int n, float rotAngle)
{
	POINT* Points = new POINT[n];

	float alfa = rotAngle; // ugao za koji treba da se rotira

	float factor = 2 * M_PI / (double)n; // kolko svaki ugao ima stepeni unutar figure sa n stranica

	for (int i = 0; i < n; i++)
	{
		// nalazimo koordinate svakog temena
		Points[i].x = (int)(cx + r * cos(alfa));
		Points[i].y = (int)(cy + r * sin(alfa));

		// svako teme ce da bude pomereno za factor pa samim tim povecavamo alfa 
		alfa += factor;
	}

	pDC->Polygon(Points, n);

	delete[] Points;
}

Cvezba1Doc* Cvezba1View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cvezba1Doc)));
	return (Cvezba1Doc*)m_pDocument;
}
#endif //_DEBUG


// Cvezba1View message handlers

void Cvezba1View::OnLButtonDown(UINT nFlags, CPoint point)
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

			// 2. Zaokruzivanje na najblizih 0.5
			double kolona = floor(rawX * 2 + 0.5) / 2.0;
			double vrsta = floor(rawY * 2 + 0.5) / 2.0;

			CString msg;
			msg.Format(L"(%.1f, %.1f)", kolona, vrsta);
			AfxMessageBox(msg);
		}
	}

	CView::OnLButtonDown(nFlags, point);
}
