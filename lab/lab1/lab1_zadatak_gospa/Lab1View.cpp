
// Lab1View.cpp : implementation of the CLab1View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Lab1.h"
#endif

#include "Lab1Doc.h"
#include "Lab1View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define WIDTH 500
#define HEIGHT 500
#define NUMBER_OF_SQUARES 20
#define SQUARE_SIZE (WIDTH / NUMBER_OF_SQUARES)


// CLab1View

IMPLEMENT_DYNCREATE(CLab1View, CView)

BEGIN_MESSAGE_MAP(CLab1View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CLab1View construction/destruction

CLab1View::CLab1View() noexcept
{
	// TODO: add construction code here

}

CLab1View::~CLab1View()
{
}

BOOL CLab1View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CLab1View drawing

void CLab1View::OnDraw(CDC* pDC)
{
    CLab1Doc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    // Gray background
    CBrush brushBg(RGB(221, 221, 221));
    CBrush* oldBrush = pDC->SelectObject(&brushBg);
    pDC->FillRect(CRect(0, 0, WIDTH, HEIGHT), &brushBg);
    pDC->SelectObject(oldBrush);
    pDC->SetBkMode(TRANSPARENT);

    // Pen for outlines
    CPen pen;
    pen.CreatePen(PS_SOLID, 3.5, RGB(0, 0, 255));
    CPen* oldPen = pDC->SelectObject(&pen);

    POINT center;
    vector<POINT> points;

    // Red tringle
    points = { GridPoint(1, 6), GridPoint(5, 6), GridPoint(5, 2) };
    center = CenterOfThreeDots(points);
    DrawPolygon(pDC, points, RGB(255, 0, 0));
    DrawRegularPolygon(pDC, center.x, center.y, SQUARE_SIZE / 1.3, 6, 0.0f);

    // Orange parallelogram
    points = { GridPoint(1, 6), GridPoint(7, 6), GridPoint(10, 9), GridPoint(4, 9) };
    DrawPolygon(pDC, points, RGB(255, 153, 51));


    // White square with blue stripes
    points = { GridPoint(13, 6), GridPoint(10, 9), GridPoint(7, 6), GridPoint(10, 3) };
    DrawPolygon(pDC, points, -1, HS_CROSS, RGB(182, 206, 255), RGB(255, 255, 255));

    // Yellow triangle
    points = { GridPoint(10, 3), GridPoint(13, 6), GridPoint(16, 3) };
    center = CenterOfThreeDots(points);
    DrawPolygon(pDC, points, RGB(255, 255, 0));
    DrawRegularPolygon(pDC, center.x, center.y, SQUARE_SIZE / 1.3, 8, 0.0f);

    // Green triangle
    points = { GridPoint(16, 3), GridPoint(12, 7), GridPoint(20, 7) };
    center = CenterOfThreeDots(points);
    DrawPolygon(pDC, points, RGB(0, 255, 0));
    DrawRegularPolygon(pDC, center.x, center.y, SQUARE_SIZE / 1.3, 6, 0.0f);

    // Purple triangle
    points = { GridPoint(12, 7), GridPoint(20, 7), GridPoint(20, 15) };
    center = CenterOfThreeDots(points);
    DrawPolygon(pDC, points, RGB(153, 0, 204));
    DrawRegularPolygon(pDC, center.x, center.y, SQUARE_SIZE * 1.2, 5, 0.0f);

    // Pink triangle
    points = { GridPoint(12, 7), GridPoint(12, 19), GridPoint(18, 13) };
    center = CenterOfThreeDots(points);
    DrawPolygon(pDC, points, RGB(255, 153, 204));
    DrawRegularPolygon(pDC, center.x, center.y, SQUARE_SIZE * 1.2, 4, 0.0f);

    // Restore old pen
    pDC->SelectObject(oldPen);

    if (m_showGrid)
        DrawGrid(pDC, 0, 0, WIDTH, HEIGHT, SQUARE_SIZE);
}

void CLab1View::DrawRegularPolygon(CDC* pDC, int cx, int cy, int r, int n, float rotAngleDegrees)
{
    if (n < 3) return;

    vector<POINT> pts;
    pts.reserve(n);

    double rot = rotAngleDegrees * (M_PI / 180.0);

    for (int i = 0; i < n; ++i) {
        double alfa = rot + (2.0 * M_PI * i) / n;
        double x = r * cos(alfa);
        double y = r * sin(alfa);

        int px = cx + static_cast<int>(round(x));
        int py = cy - static_cast<int>(round(y));

        pts.push_back({ px, py });
    }

    CBrush* oldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
    pDC->Polygon(pts.data(), (int)pts.size());
    pDC->SelectObject(oldBrush);
}

void CLab1View::DrawPolygon(CDC* pDC, const vector<POINT>& pts, COLORREF fillColor, int hatchStyle, COLORREF hatchFg, COLORREF hatchBg)
{
    if (pts.size() < 3) return;

    if (hatchStyle == -1)
    {
        CBrush solidBrush(fillColor);
        CBrush* oldBrush = pDC->SelectObject(&solidBrush);

        pDC->Polygon(pts.data(), (int)pts.size());

        pDC->SelectObject(oldBrush);
        return;
    }

    int oldBkMode = pDC->SetBkMode(OPAQUE);
    COLORREF oldBkColor = pDC->SetBkColor(hatchBg);

    CBrush hatchBrush;
    hatchBrush.CreateHatchBrush(hatchStyle, hatchFg);

    CBrush* oldBrush = pDC->SelectObject(&hatchBrush);

    pDC->Polygon(pts.data(), (int)pts.size());

    pDC->SelectObject(oldBrush);
    pDC->SetBkMode(oldBkMode);
    pDC->SetBkColor(oldBkColor);
}

void CLab1View::DrawGrid(CDC* pDC, int left, int top, int width, int height, int spacing)
{
    CPen penGrid;
    penGrid.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    CPen* oldPen = pDC->SelectObject(&penGrid);

    for (int x = left; x <= left + width; x += spacing) {
        pDC->MoveTo(x, top);
        pDC->LineTo(x, top + height);
    }

    for (int y = top; y <= top + height; y += spacing) {
        pDC->MoveTo(left, y);
        pDC->LineTo(left + width, y);
    }

    pDC->SelectObject(oldPen);
}

double CLab1View::Distance(POINT a, POINT b)
{
    double dx = ((double)b.x - a.x);
    double dy = ((double)b.y - a.y);

    return sqrt(dx * dx + dy * dy);
}

POINT CLab1View::CenterOfThreeDots(POINT va, POINT vb, POINT vc)
{
    double a = this->Distance(vb, vc);
    double b = this->Distance(va, vc);
    double c = this->Distance(va, vb);
    double denom = a + b + c;

    if (denom == 0.0)
        return { va.x, va.y };

    return {
        (LONG)round((va.x * a + vb.x * b + vc.x * c) / denom),
        (LONG)round((va.y * a + vb.y * b + vc.y * c) / denom)
    };
}

POINT CLab1View::CenterOfThreeDots(vector<POINT> pts)
{
    return CenterOfThreeDots(pts[0], pts[1], pts[2]);
}

POINT CLab1View::GridPoint(double gx, double gy)
{
    return { (LONG)round(gx * SQUARE_SIZE), (LONG)round(gy * SQUARE_SIZE) };
}

void CLab1View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == 'G' || nChar == 'g')
    {
        m_showGrid = !m_showGrid;
        Invalidate();
        return;
    }

    CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

// CLab1View printing

BOOL CLab1View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CLab1View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CLab1View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CLab1View diagnostics

#ifdef _DEBUG
void CLab1View::AssertValid() const
{
	CView::AssertValid();
}

void CLab1View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLab1Doc* CLab1View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLab1Doc)));
	return (CLab1Doc*)m_pDocument;
}
#endif //_DEBUG


// CLab1View message handlers
