
// TextDisplayView.h: CTextDisplayView 클래스의 인터페이스
//

#pragma once
#include <ddraw.h>

class CTextDisplayView : public CView
{
protected: // serialization에서만 만들어집니다.
	CTextDisplayView() noexcept;
	DECLARE_DYNCREATE(CTextDisplayView)

// 특성입니다.
public:
	CTextDisplayDoc* GetDocument() const;

	LPDIRECTDRAW7 m_pDDObj;
	LPDIRECTDRAWSURFACE7 m_pPS;
	LPDIRECTDRAWSURFACE7 m_pBB;
	BOOL m_DDrawOK;
	CString m_TitleMsg;
	CString m_PressMsg;
	CString m_FrontMsg;
	CString m_BackMsg;
	CString m_EndMsg;

// 작업입니다.
public:
	BOOL CreateSurfaces(void);
	BOOL ClearSurface(LPDIRECTDRAWSURFACE7 pSurface);
	void UpdateSurface(void);


// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CTextDisplayView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnInitialUpdate();
};

#ifndef _DEBUG  // TextDisplayView.cpp의 디버그 버전
inline CTextDisplayDoc* CTextDisplayView::GetDocument() const
   { return reinterpret_cast<CTextDisplayDoc*>(m_pDocument); }
#endif

