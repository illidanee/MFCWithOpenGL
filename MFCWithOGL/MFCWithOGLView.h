
// MFCWithOGLView.h : interface of the CMFCWithOGLView class
//

#pragma once

#include <GL/glew.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")

#include <glm/glm.hpp>
#include <glm/ext.hpp>

class CMFCWithOGLView : public CView
{
protected: // create from serialization only
	CMFCWithOGLView();
	DECLARE_DYNCREATE(CMFCWithOGLView)

// Attributes
public:
	CMFCWithOGLDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CMFCWithOGLView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()


private:
	bool InitOpenGL();
	void RenderGL();
	const char* LoadFileContent(const char* path);
	GLuint CreateGPUProgram(const char* vsShaderPath, const char* fsShaderPath);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	CDC* m_pDC;
	HGLRC m_GLRC;

	float identity[16];
	glm::mat4 projection;

	int m_ViewWidth;
	int m_ViewHeight;

	GLuint program;
	GLuint posLocation, colorLocation, MLocation, VLocation, PLocation;
	GLuint vbo;
	GLuint vio;
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#ifndef _DEBUG  // debug version in MFCWithOGLView.cpp
inline CMFCWithOGLDoc* CMFCWithOGLView::GetDocument() const
   { return reinterpret_cast<CMFCWithOGLDoc*>(m_pDocument); }
#endif

