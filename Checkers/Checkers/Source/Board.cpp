#include "Board.h"
#include <assert.h>
#include "Simulation.h"

static Simulation* m_sim = NULL;
void Board::Update(float _dt )
{
	// Update highlight 
	int mouseCellPos = GetCell( m_sim->GetMousePos() );
	if( CheckerOnCell(mouseCellPos) )
	{
		m_mouseHighlight.SetPos( GetCellPos(  mouseCellPos) );
		m_mouseHighlight.SetRenderState(true);
	}
	else
	{
		m_mouseHighlight.SetRenderState(false);
	}
}

void Board::Cleanup()
{
	if(m_image != 0)
	{
		glDeleteTextures(1, &m_image);
	}

	if(m_image != 0)
	{
		glDeleteTextures(1, &m_checkerPlain);
	}

	if(m_image != 0)
	{
		glDeleteTextures(1, &m_checkerKing);
	}

	if(m_image != 0)
	{
		glDeleteTextures(1, &m_cellHighlight);
	}
}

void Board::Render()
{
	if(m_canRender)
	{
		// Draw board
		DrawImage(m_image, Color(1,1,1,1), m_screenPos.x, m_screenPos.y, m_imageRes.x, m_imageRes.y);
		
		// Drawhighlight of cell that has mouse and checker on it
		m_mouseHighlight.Render_IgnoreBehind();

		// Draw checkers and highlights
		for(int i=0; i<12; ++i)
		{
			m_cellHighLights[i].Render_IgnoreBehind();
			m_redCheckers[i].Render();
			m_blackCheckers[i].Render();
		}
	}
}

bool Board::CheckerOnCell( int _cell )
{
	if( _cell >= 0 && _cell < 8 * 8)
	{
		int cell = -1;
		for(int i=0; i<12; ++i)
		{
			cell = GetCell( m_redCheckers[i].GetPos() );
			if( cell != -1 && cell == _cell)
			{
				return true;
			}
			cell = GetCell( m_blackCheckers[i].GetPos() );
			if( cell != -1 && cell == _cell)
			{
				return true;
			}
		}
	}
	return false;
}


int	Board::GetCell( glm::vec2 _screenPos)
{
	int retval = -1;
	if(_screenPos.x >= m_boardRect.upperLeft.x
		&& _screenPos.x <= m_boardRect.botRight.x
		&& _screenPos.y >= m_boardRect.upperLeft.y
		&& _screenPos.y <= m_boardRect.botRight.y)
	{
		//Within AABB, get cell pos
		glm::vec2 modifiedBoardPos = m_screenPos - m_boardRect.upperLeft;
		glm::vec2 modified_ScreenPos = _screenPos - m_boardRect.upperLeft;
		int x = modified_ScreenPos.x / m_cellSize;
		int y = modified_ScreenPos.y / m_cellSize;
		retval = y * 8 + x;
	}
	return retval;
}

void Board::SetPos(glm::vec2 _pos)
{
	Render2DObj::SetPos(_pos);
	m_boardRect.botRight = _pos + (m_imageRes / 2.f);
	m_boardRect.upperLeft = _pos - (m_imageRes / 2.f);
}

void Board::Setup( char* _imageFilePath, glm::vec2 _imageRes, glm::vec2 _screenRes )
{
	Render2DObj::Setup(_imageFilePath, _imageRes);
	SetPos(glm::vec2(_screenRes.x/2, _screenRes.y/2));
	m_screenRes = _screenRes;

	m_cellHighlight = LoadImage("cellHighlight.png");
	m_checkerPlain = LoadImage("checkerPlain.png");
	m_checkerKing = LoadImage("checkerKing.png");

	m_mouseHighlight.Setup(m_cellHighlight, m_imageRes / 8.f, Color(1.f,.7f, 0,1));
	m_mouseHighlight.SetPos( GetCellPos( -1 ) );
	m_mouseHighlight.SetRenderState(false);

	Checker obj;
	Render2DObj highlight;
	glm::vec2 startPos = m_screenPos;
	m_cellSize = m_imageRes.x / 8.f;
	startPos -= glm::vec2(3.5f, 3.5f) * m_cellSize;
	int cellPos = -1;
	for(int i=0; i<12; ++i)
	{
		// Offsets checker by 2 horizontally but offset resets after 4 in a row (for new row)
		obj.Setup(m_checkerPlain, m_imageRes / 8.f, Color(1,0,0,1) );
		// checkers begin spaced out by 2 in their rows.
		// The middle row is the same but has an offset of 1
		cellPos = i * 2 + (i >= 4 && i < 8 ? 1 : 0);
		obj.SetPos( GetCellPos( cellPos ) );
		m_redCheckers.push_back(obj);

		obj.Setup(m_checkerPlain, m_imageRes / 8.f, Color(0.12f,0.12f,0.12f,1) );
		// Follows same rules as above cell calculation
		// but in reverse because these checkers are in mirror position
		cellPos = i * -2 + (i >= 4 && i < 8 ? -1 : 0) + 63;
		obj.SetPos( GetCellPos(cellPos) );
		m_blackCheckers.push_back(obj);

		// Setup Highlights
		highlight.Setup(m_cellHighlight, m_imageRes / 8.f, Color(0,0, 1,.7f));
		highlight.SetPos( GetCellPos( -1 ) );
		highlight.SetRenderState(false);
		m_cellHighLights.push_back(highlight);
	}

	m_sim = Simulation::GetSimulation();
}

glm::vec2 Board::GetCellPos( int c )
{
	assert( c >= -1 && c < 64);
	if( c == -1)
	{
		// give bogus position so that the object is off screen
		return glm::vec2(-m_screenPos);
	}
	// Get beginning offset
	glm::vec2 retval =	m_screenPos 
						- glm::vec2(m_imageRes.x/2.f, m_imageRes.y/2.f) 
						+ (glm::vec2(1,1) * (m_cellSize/2.f) );
	retval += glm::vec2(c % 8, floor(c/8.f)) * m_cellSize;
	return retval;
}