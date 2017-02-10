
// MFCWithOGLView.cpp : implementation of the CMFCWithOGLView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MFCWithOGL.h"
#endif

#include "MFCWithOGLDoc.h"
#include "MFCWithOGLView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//float identity[] = {
//	1, 0, 0, 0,
//	0, 1, 0, 0,
//	0, 0, 1, 0,
//	0, 0, 0, 1,
//};
//
//glm::mat4 projection = glm::perspective(45.0f, 1280.0f / 800.0f, 0.1f, 1000.0f);

struct VertexBoundle
{
	float pos[3];
	float color[4];
};


// CMFCWithOGLView

IMPLEMENT_DYNCREATE(CMFCWithOGLView, CView)

BEGIN_MESSAGE_MAP(CMFCWithOGLView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CMFCWithOGLView construction/destruction

CMFCWithOGLView::CMFCWithOGLView()
{
	// TODO: add construction code here

	m_pDC = 0;
	m_GLRC = 0;

	m_ViewWidth = 0;
	m_ViewHeight = 0;

	program = 0;
	posLocation = 0;
	colorLocation = 0;
	MLocation = 0;
	VLocation = 0;
	PLocation = 0;

	vbo = 0;
	vio = 0;
}

CMFCWithOGLView::~CMFCWithOGLView()
{
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
	wglDeleteContext(m_GLRC);
	m_GLRC = 0;
	delete m_pDC;
	m_pDC = 0;
}

BOOL CMFCWithOGLView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	return CView::PreCreateWindow(cs);
}

// CMFCWithOGLView drawing

void CMFCWithOGLView::OnDraw(CDC* /*pDC*/)
{
	CMFCWithOGLDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here

	RenderGL();
	SwapBuffers(m_pDC->GetSafeHdc());
}


// CMFCWithOGLView printing

BOOL CMFCWithOGLView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMFCWithOGLView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMFCWithOGLView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CMFCWithOGLView diagnostics

#ifdef _DEBUG
void CMFCWithOGLView::AssertValid() const
{
	CView::AssertValid();
}

void CMFCWithOGLView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCWithOGLDoc* CMFCWithOGLView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCWithOGLDoc)));
	return (CMFCWithOGLDoc*)m_pDocument;
}
#endif //_DEBUG


// CMFCWithOGLView message handlers



bool CMFCWithOGLView::InitOpenGL()
{
	m_pDC = new CClientDC(this);

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL;
	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;

	int iPixelFormat = ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd);
	BOOL bRet = SetPixelFormat(m_pDC->GetSafeHdc(), iPixelFormat, &pfd);
	m_GLRC = wglCreateContext(m_pDC->GetSafeHdc());
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_GLRC);

	return true;
}

void CMFCWithOGLView::RenderGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);
	glUniformMatrix4fv(MLocation, 1, GL_FALSE, identity);
	glUniformMatrix4fv(VLocation, 1, GL_FALSE, identity);
	glUniformMatrix4fv(PLocation, 1, GL_FALSE, glm::value_ptr(projection));

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(posLocation);
	glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexBoundle), (const void*)0);
	glEnableVertexAttribArray(colorLocation);
	glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoundle), (const void*)(sizeof(float) * 3));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vio);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	glUseProgram(0);
}

const char* CMFCWithOGLView::LoadFileContent(const char* path)
{
	FILE* pf = fopen(path, "rb");
	if (pf == 0)
		return 0;

	fseek(pf, 0, SEEK_END);
	long len = ftell(pf);
	rewind(pf);
	char* buffer = new char[len + 1];
	fread(buffer, 1, len, pf);
	buffer[len] = 0;
	fclose(pf);

	return buffer;
}

GLuint CMFCWithOGLView::CreateGPUProgram(const char* vsShaderPath, const char* fsShaderPath)
{
	GLuint vsShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fsShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* vsCode = LoadFileContent(vsShaderPath);
	const char* fsCode = LoadFileContent(fsShaderPath);
	glShaderSource(vsShader, 1, &vsCode, 0);
	glShaderSource(fsShader, 1, &fsCode, 0);
	glCompileShader(vsShader);
	glCompileShader(fsShader);
	GLuint pragram = glCreateProgram();
	glAttachShader(pragram, vsShader);
	glAttachShader(pragram, fsShader);
	glLinkProgram(pragram);
	glDetachShader(pragram, vsShader);
	glDetachShader(pragram, fsShader);
	glDeleteShader(vsShader);
	glDeleteShader(fsShader);
	return pragram;
}

int CMFCWithOGLView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	
	InitOpenGL();
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.3f, 0.4f, 1.0f);

	program = CreateGPUProgram("sample.vs", "sample.fs");
	
	posLocation = glGetAttribLocation(program, "pos");
	colorLocation = glGetAttribLocation(program, "color");
	MLocation = glGetUniformLocation(program, "M");
	VLocation = glGetUniformLocation(program, "V");
	PLocation = glGetUniformLocation(program, "P");

	//上传顶点数据到GPU
	VertexBoundle vertex[3];
	vertex[0].pos[0] = 0.0f;
	vertex[0].pos[1] = 0.0f;
	vertex[0].pos[2] = -100.0f;
	vertex[0].color[0] = 1.0f;
	vertex[0].color[1] = 1.0f;
	vertex[0].color[2] = 1.0f;
	vertex[0].color[3] = 1.0f;

	vertex[1].pos[0] = 10.0f;
	vertex[1].pos[1] = 0.0f;
	vertex[1].pos[2] = -100.0f;
	vertex[1].color[0] = 1.0f;
	vertex[1].color[1] = 1.0f;
	vertex[1].color[2] = 1.0f;
	vertex[1].color[3] = 1.0f;

	vertex[2].pos[0] = 0.0f;
	vertex[2].pos[1] = 10.0f;
	vertex[2].pos[2] = -100.0f;
	vertex[2].color[0] = 1.0f;
	vertex[2].color[1] = 1.0f;
	vertex[2].color[2] = 1.0f;
	vertex[2].color[3] = 1.0f;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexBoundle) * 100, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VertexBoundle) * 3, vertex);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	unsigned int indexes[] = { 0, 1, 2 };
	glGenBuffers(1, &vio);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vio);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 3, indexes, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return 0;
}


void CMFCWithOGLView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	if (cy == 0)
		return;

	m_ViewWidth = cx;
	m_ViewHeight = cy;

	for (int i = 0; i < 16; ++i)
		identity[i] = 0;

	identity[0] = 1;
	identity[5] = 1;
	identity[10] = 1;
	identity[15] = 1;

	projection = glm::perspective(45.0f, (float)m_ViewWidth / m_ViewHeight, 0.1f, 1000.0f);

	glViewport(0, 0, m_ViewWidth, m_ViewHeight);
}


BOOL CMFCWithOGLView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return  TRUE;
	//return CView::OnEraseBkgnd(pDC);
}
