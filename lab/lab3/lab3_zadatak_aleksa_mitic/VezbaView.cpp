
// VezbaView.cpp : implementation of the CVezbaView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Vezba.h"
#endif

#include "VezbaDoc.h"
#include "VezbaView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVezbaView

IMPLEMENT_DYNCREATE(CVezbaView, CView)

BEGIN_MESSAGE_MAP(CVezbaView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CVezbaView construction/destruction

// Ucitavamo sve puzle
CVezbaView::CVezbaView() noexcept
{
	char s[] = "1.bmp";
	for (int i = 0; i < MAX_PIECES; i++) {
		s[0] = '0' + (i + 1);
		m_pieces[i].img = new DImage();
		m_pieces[i].img->Load(CString(s));

		// Postavi ih na pocetne pozicije (npr. sve u vrhu ili random)
		m_pieces[i].id = i;
		m_pieces[i].x = 100 + (i % 3) * 150;
		m_pieces[i].y = 100 + (i / 3) * 150;
		m_pieces[i].angle = 0;
		m_pieces[i].mx = m_pieces[i].my = false;
		m_pieces[i].isBlue = (i == 0); 
	}
}

CVezbaView::~CVezbaView()
{
	for (int i = 0; i < MAX_PIECES; i++) {
		if (m_pieces[i].img) {
			delete m_pieces[i].img;
			m_pieces[i].img = nullptr;
		}
	}
}

BOOL CVezbaView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}


void CVezbaView::DrawTransparent(CDC* pDC, DImage* img, bool isBlue) {

	int width = img->Width();
	int height = img->Height();

	CBitmap srcBitmap;
	srcBitmap.CreateCompatibleBitmap(pDC, width, height);
	CBitmap maskBitmap;
	maskBitmap.CreateBitmap(width, height, 1, 1, nullptr);

	CDC srcDC, dstDC;
	srcDC.CreateCompatibleDC(pDC);
	dstDC.CreateCompatibleDC(pDC);

	CBitmap* oldSrcBitmap = srcDC.SelectObject(&srcBitmap);
	CBitmap* oldDstBitmap = dstDC.SelectObject(&maskBitmap);

	// 1. Crtamo originalnu sliku u srcDC
	img->Draw(&srcDC, CRect(0, 0, width, height), CRect(0, 0, width, height));

	// 2. KLJU?NO: Uzimamo boju pozadine PRE modifikacija
	COLORREF trColor = srcDC.GetPixel(0, 0);

	// 3. Modifikujemo boje, ali šaljemo trColor da je funkcije ne bi dirale
	if (isBlue)
		makeBlue(&srcBitmap, trColor);
	else
		makeGray(&srcBitmap, trColor);

	// 4. Pravimo masku (trColor je sad sigurno ista ona boja koju smo uzeli na po?etku)
	COLORREF oldBgColor = srcDC.SetBkColor(trColor);
	dstDC.BitBlt(0, 0, width, height, &srcDC, 0, 0, SRCCOPY);

	// 5. Priprema izvora za lepljenje (pozadina u crno)
	srcDC.SetTextColor(RGB(255, 255, 255));
	srcDC.SetBkColor(RGB(0, 0, 0));
	srcDC.BitBlt(0, 0, width, height, &dstDC, 0, 0, SRCAND);

	// 6. Finalno crtanje na ekran
	Translate(pDC, -width / 2, -height / 2);
	pDC->BitBlt(0, 0, width, height, &dstDC, 0, 0, SRCAND);   // rupa
	pDC->BitBlt(0, 0, width, height, &srcDC, 0, 0, SRCPAINT); // boja
	Translate(pDC, width / 2, height / 2);

	srcDC.SelectObject(oldSrcBitmap);
	dstDC.SelectObject(oldDstBitmap);
}
void CVezbaView::makeGray(CBitmap* bitmap, COLORREF trColor) {
	BITMAP b;
	bitmap->GetBitmap(&b);
	int bytesPerPixel = b.bmBitsPixel / 8;
	BYTE* bits = new BYTE[b.bmWidthBytes * b.bmHeight];
	bitmap->GetBitmapBits(b.bmWidthBytes * b.bmHeight, bits);

	BYTE bTr = GetBValue(trColor);
	BYTE gTr = GetGValue(trColor);
	BYTE rTr = GetRValue(trColor);

	// Prag tolerancije - koliko piksel sme da odstupa od pozadine (npr. 10-20)
	int threshold = 15;

	for (int y = 0; y < b.bmHeight; y++) {
		for (int x = 0; x < b.bmWidth; x++) {
			long i = y * b.bmWidthBytes + x * bytesPerPixel;

			// Provera sa tolerancijom: Da li je piksel skoro isti kao pozadina?
			if (abs(bits[i] - bTr) < threshold &&
				abs(bits[i + 1] - gTr) < threshold &&
				abs(bits[i + 2] - rTr) < threshold)
			{
				// AKO JE "PRLJAVA" POZADINA -> O?isti je na ?istu boju pozadine
				bits[i] = bTr;
				bits[i + 1] = gTr;
				bits[i + 2] = rTr;
				continue;
			}

			// Ako nije pozadina, pravi sivu boju
			BYTE gr = min(255, (bits[i] + bits[i + 1] + bits[i + 2]) / 3 + 64);
			bits[i] = bits[i + 1] = bits[i + 2] = gr;
		}
	}
	bitmap->SetBitmapBits(b.bmWidthBytes * b.bmHeight, bits);
	delete[] bits;
}
void CVezbaView::makeBlue(CBitmap* bitmap, COLORREF trColor) {
	BITMAP b;
	bitmap->GetBitmap(&b);
	int bytesPerPixel = b.bmBitsPixel / 8;
	BYTE* bits = new BYTE[b.bmWidthBytes * b.bmHeight];
	bitmap->GetBitmapBits(b.bmWidthBytes * b.bmHeight, bits);

	BYTE bTr = GetBValue(trColor);
	BYTE gTr = GetGValue(trColor);
	BYTE rTr = GetRValue(trColor);
	int threshold = 15;

	for (int y = 0; y < b.bmHeight; y++) {
		for (int x = 0; x < b.bmWidth; x++) {
			long i = y * b.bmWidthBytes + x * bytesPerPixel;

			if (abs(bits[i] - bTr) < threshold &&
				abs(bits[i + 1] - gTr) < threshold &&
				abs(bits[i + 2] - rTr) < threshold)
			{
				bits[i] = bTr;
				bits[i + 1] = gTr;
				bits[i + 2] = rTr;
				continue;
			}

			BYTE gr = min(255, (bits[i] + bits[i + 1] + bits[i + 2]) / 3 + 64);
			bits[i] = gr;
			bits[i + 1] = 0;
			bits[i + 2] = 0;
		}
	}
	bitmap->SetBitmapBits(b.bmWidthBytes * b.bmHeight, bits);
	delete[] bits;
}

void CVezbaView::DrawGrid(CDC* pDC)
{
	CPen pen(PS_SOLID, 2, RGB(229, 229, 229));
	CPen* old = pDC->SelectObject(&pen);

	for (int i = 0; i <= 500; i += 25)
	{
		pDC->MoveTo(0, i);
		pDC->LineTo(500, i);

		pDC->MoveTo(i, 0);
		pDC->LineTo(i, 500);
	}
	pDC->SelectObject(old);
}
void CVezbaView::DrawBackground(COLORREF backgroundColor, CDC* pDC)
{
	CBrush brush(backgroundColor);
	CRect client;
	GetClientRect(&client);

	pDC->FillRect(client, &brush);
}
void CVezbaView::Translate(CDC* pDC, float dx, float dy, bool rightMultiply)
{
	XFORM form = { 1, 0, 0, 1, dx, dy };
	pDC->ModifyWorldTransform(&form, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}
void CVezbaView::Scale(CDC* pDC, float sx, float sy, bool rightMultiply)
{
	XFORM form = { sx, 0, 0, sy, 0, 0 };
	pDC->ModifyWorldTransform(&form, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}
void CVezbaView::Mirror(CDC* pDC, bool mx, bool my, bool rightMyltiply)
{
	Scale(pDC, mx ? -1 : 1, my ? -1 : 1, rightMyltiply);
}
void CVezbaView::Rotate(CDC* pDC, float angle, bool rightMultiply)
{
	float sindeg = sin(angle);
	float cosdeg = cos(angle);
	XFORM form = { cosdeg, sindeg, -sindeg, cosdeg, 0, 0 };
	pDC->ModifyWorldTransform(&form, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CVezbaView::DrawMem(CDC* pDC)
{
	DrawBackground(RGB(255, 255, 255), pDC);
	DrawGrid(pDC);

	XFORM oldTr;
	pDC->GetWorldTransform(&oldTr);

	for (int i = 0; i < MAX_PIECES; i++) {
		Translate(pDC, m_pieces[i].x, m_pieces[i].y);
		Rotate(pDC, m_pieces[i].angle);
		Mirror(pDC, m_pieces[i].mx, m_pieces[i].my);

		DrawTransparent(pDC, m_pieces[i].img, m_pieces[i].isBlue);

		// Ako je deo selektovan crtam okvir 
		if (i == m_selectedIndex) {
			CPen pen(PS_DOT, 1, RGB(255, 0, 0));
			CPen* oldPen = pDC->SelectObject(&pen);
			pDC->MoveTo(-55, -55); // okvir malo ve?i od dela
			pDC->LineTo(55, -55); pDC->LineTo(55, 55);
			pDC->LineTo(-55, 55); pDC->LineTo(-55, -55);
			pDC->SelectObject(oldPen);
		}

		pDC->SetWorldTransform(&oldTr); // Reset transformacije za sledeci deo
	}

	if (m_showInfo && m_selectedIndex != -1) {
		// Priprema podataka o selektovanom delu
		Piece& p = m_pieces[m_selectedIndex];
		CString strId, strPos, strRot, strMirror;

		strId.Format(L"Selektovan deo: %d", p.id + 1);
		strPos.Format(L"Pozicija: X=%.1f, Y=%.1f", p.x, p.y);
		strRot.Format(L"Rotacija: %.1f stepeni", DEG(p.angle)); // Pretvaramo u stepene
		strMirror.Format(L"Mirror: X=%s, Y=%s", p.mx ? L"DA" : L"NE", p.my ? L"DA" : L"NE");

		// Crtanje polu-providnog panela 
		CRect panelRect(800, 20, 1030, 140); // Postavljamo panel desno od grida (grid je do 500)
		CBrush panelBrush(RGB(240, 240, 240));
		CPen panelPen(PS_SOLID, 1, RGB(100, 100, 100));

		CBrush* oldBrush = pDC->SelectObject(&panelBrush);
		CPen* oldPen = pDC->SelectObject(&panelPen);

		pDC->Rectangle(panelRect); // Crtamo pozadinu panela

		// Ispis teksta
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(0, 0, 0));

		int tx = 810;
		pDC->TextOutW(tx, 30, strId);
		pDC->TextOutW(tx, 55, strPos);
		pDC->TextOutW(tx, 80, strRot);
		pDC->TextOutW(tx, 105, strMirror);

		// Vracanje starih objekata
		pDC->SelectObject(oldBrush);
		pDC->SelectObject(oldPen);
	}

}


void CVezbaView::OnDraw(CDC* pDC)
{
	CVezbaDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CRect rect;
	GetClientRect(&rect);

	int pDCmode = pDC->SetGraphicsMode(GM_ADVANCED);

	CDC* memDC = new CDC(); // kontekst koji sluzi za crtanje u memoriju
	memDC->CreateCompatibleDC(pDC);

	int memDCmode = memDC->SetGraphicsMode(GM_ADVANCED);

	CBitmap memBitmap; // cuva sliku u vidu piksela, sluzi za crtanje u memoriju
	memBitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	CBitmap* old = memDC->SelectObject(&memBitmap);

	DrawMem(memDC);

	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), memDC, 0, 0, SRCCOPY);

	pDC->SetGraphicsMode(pDCmode);
	memDC->SetGraphicsMode(memDCmode);
	memDC->SelectObject(old);
	memDC->DeleteDC();
	delete memDC;
}







// CVezbaView printing
BOOL CVezbaView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}
void CVezbaView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}
void CVezbaView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

// CVezbaView diagnostics
#ifdef _DEBUG
void CVezbaView::AssertValid() const
{
	CView::AssertValid();
}

void CVezbaView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CVezbaDoc* CVezbaView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVezbaDoc)));
	return (CVezbaDoc*)m_pDocument;
}
#endif //_DEBUG





// CVezbaView message handlers
void CVezbaView::OnLButtonDown(UINT nFlags, CPoint point)
{
	int oldSelected = m_selectedIndex; // ?uvamo prethodnu selekciju
	m_selectedIndex = -1;              // Resetujemo selekciju (pretpostavljamo klik u prazno)
	bool found = false;

	// Prolazimo kroz delove od poslednjeg ka prvom (da bi kliknuli na onaj koji je "odozgo")
	for (int i = MAX_PIECES - 1; i >= 0; i--) {
		// Hit-test (provera da li je klik unutar kvadrata 100x100 oko centra dela)
		CRect rect(m_pieces[i].x - 50, m_pieces[i].y - 50, m_pieces[i].x + 50, m_pieces[i].y + 50);

		if (rect.PtInRect(point)) {
			m_selectedIndex = i;
			m_isDragging = true;
			m_lastMousePos = point;

			// Pomeranje selektovanog dela na kraj niza (Z-order: da bude iznad ostalih)
			Piece temp = m_pieces[i];
			for (int j = i; j < MAX_PIECES - 1; j++) m_pieces[j] = m_pieces[j + 1];
			m_pieces[MAX_PIECES - 1] = temp;
			m_selectedIndex = MAX_PIECES - 1;

			found = true;
			break; // Prekidamo petlju jer smo našli deo
		}
	}

	// Ako se selekcija promenila (bilo da smo selektovali novi ili odselektovali stari), osveži ekran
	if (m_selectedIndex != oldSelected) {
		Invalidate();
	}

	CView::OnLButtonDown(nFlags, point);
}
void CVezbaView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_isDragging && m_selectedIndex != -1) {
		Piece& p = m_pieces[m_selectedIndex];

		float targetX = 100 + (p.id % 3) * 150;
		float targetY = 100 + (p.id / 3) * 150;

		
		float dist = sqrt(pow(p.x - targetX, 2) + pow(p.y - targetY, 2));

		
		if (dist < 40.0f) {
			p.x = targetX;
			p.y = targetY;

			
			float angleDeg = DEG(p.angle);
			while (angleDeg > 180) angleDeg -= 360;
			while (angleDeg <= -180) angleDeg += 360;

			// Ako je blizu 0 stepeni (dozvoljena greška 10 stepeni)
			if (fabs(angleDeg) < 10.0f) {
				p.angle = 0;   
				p.mx = false;  
				p.my = false; 
			}

			Invalidate();
		}
	}

	m_isDragging = false;
	CView::OnLButtonUp(nFlags, point);
}
void CVezbaView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_isDragging && m_selectedIndex != -1) {
		CSize delta = point - m_lastMousePos;
		m_pieces[m_selectedIndex].x += delta.cx;
		m_pieces[m_selectedIndex].y += delta.cy;
		m_lastMousePos = point;
		Invalidate(); // Ponovo iscrtaj
	}

	CView::OnMouseMove(nFlags, point);
}
void CVezbaView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_selectedIndex == -1) return;

	bool shiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
	float moveStep = shiftPressed ? 1.0f : 10.0f;
	float angleStep = shiftPressed ? RAD(0.1) : RAD(5.0);

	switch (nChar) {
		case VK_LEFT:
			m_pieces[m_selectedIndex].x -= moveStep;
			break;
		case VK_RIGHT:
			m_pieces[m_selectedIndex].x += moveStep;
			break;
		case VK_UP:
			m_pieces[m_selectedIndex].y -= moveStep;
			break;
		case VK_DOWN:
			m_pieces[m_selectedIndex].y += moveStep;
			break;
		case 'A':
			m_pieces[m_selectedIndex].angle -= angleStep;
			break;
		case 'D':
			m_pieces[m_selectedIndex].angle += angleStep;
			break;
		case 'X':
			m_pieces[m_selectedIndex].mx = !m_pieces[m_selectedIndex].mx;
			break;
		case 'Y':
			m_pieces[m_selectedIndex].my = !m_pieces[m_selectedIndex].my;
			break;
		case 'O':
			m_showInfo = !m_showInfo;
			break;
	}
	Invalidate();

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CVezbaView::OnEraseBkgnd(CDC* pDC)
{
	return true;
}
