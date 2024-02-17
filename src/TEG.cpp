#include "TEG.h"
#include <filesystem>
#include <ncurses.h>
#include <fstream>

TEG::TEG( const std::string& fileName )
{
  m_fileName = fileName;
  initscr();
  noecho();
  cbreak();
  keypad( stdscr, true );
  mousemask( BUTTON1_PRESSED, NULL );

  m_currentX = m_currentY = m_shiftY = 0;
  m_mode = 'e';
  m_status = "EDIT";
}

TEG::~TEG()
{
  refresh();
  endwin();
}

void TEG::Start()
{
  if ( std::filesystem::exists( m_fileName ) )
    Open();
  else
    m_lines.push_back( "" );

  Print();
  StatusLine();

  while ( m_mode != 'q' )
  {
    Input( );
    StatusLine();
    Print();
  }
}

void TEG::Update()
{
  switch ( m_mode )
  {
    case 27:
    case 'm':
      m_status = "MENU";
      break;

    case 'e':
      m_status = "EDIT";
      break;

    case 'q':
      break;
  }
}

void TEG::StatusLine()
{
  std::string statusLine;

  if ( m_mode == 'm' )
    statusLine = m_status + "          E - EDIT MODE     S - SAVE FILE     Q - EXIT";
  else
    statusLine = m_status + "          ESC - MENU";

  move( LINES - 1, 0 );
  clrtoeol();
  attron( A_REVERSE );
  printw( statusLine.c_str() );
  attroff( A_REVERSE );
  move( m_currentY - m_shiftY, m_currentX );
}

void TEG::Input( )
{
  int ch = getch();

  switch ( ch )
  {
    case KEY_UP:
      Up();
      return;

    case KEY_LEFT:
      Left();
      return;

    case KEY_RIGHT:
      Right();
      return;

    case KEY_DOWN:
      Down();
      return;

    case KEY_MOUSE:
      MEVENT event;

      if ( getmouse( &event ) == OK )
      {
        if ( event.bstate & BUTTON1_PRESSED )
        {
          if ( ( event.y < m_lines.size() ) && ( event.x <= m_lines[event.y].length() - 1 ) )
          {
            if ( event.y + m_shiftY > m_lines.size() )
              return;

            m_currentY = event.y + m_shiftY;

            if ( event.x > m_lines[m_currentY].length() )
              return;

            m_currentX = event.x;
            move( m_currentY - m_shiftY, m_currentX );
          }
        }
      }

      return;

    case 588:
      return;

    case 589:
      return;
  }

  switch ( m_mode )
  {
    case 27:
      break;

    case 'm':

      switch ( ch )
      {
        case 'q':
          m_mode = 'q';
          Update();
          break;

        case 'e':

          m_mode = 'e';
          Update();
          break;

        case 's':
          try
          {
            Save();
            m_status = "SEVE COMPLITE";
          }
          catch ( std::exception& exept )
          {
            m_status = exept.what();
          }

          break;
      }

      break;

    case 'e':
      m_status = "EDIT";

      switch ( ch )
      {
        case 27:
          m_mode = 'm';
          Update();
          break;

        case 127:
        case KEY_BACKSPACE:
          if ( m_currentX == 0 && m_currentY > 0 )
          {
            m_currentX = m_lines[m_currentY - 1].length();
            m_lines[m_currentY - 1] += m_lines[m_currentY];
            Remove( m_currentY );
            Up();
          }
          else if ( m_currentX > 0 )
            m_lines[m_currentY].erase( --m_currentX, 1 );

          break;

        case KEY_DC:
          if ( m_currentX == m_lines[m_currentY].length() && m_currentY != m_lines.size() - 1 )
            m_lines[m_currentY] += m_lines[m_currentY + 1];

          else
            m_lines[m_currentY].erase( m_currentX, 1 );

          break;

        case KEY_ENTER:
        case 10:
          if ( m_currentX < m_lines[m_currentY].length() )
          {
            Insert( m_lines[m_currentY].substr( m_currentX, m_lines[m_currentY].length() - m_currentX ), m_currentY + 1 );
            m_lines[m_currentY].erase( m_currentX, m_lines[m_currentY].length() - m_currentX );
          }
          else
            Insert( "", m_currentY + 1 );

          m_currentX = 0;
          Down();
          break;

        case KEY_BTAB:
        case KEY_CTAB:
        case KEY_STAB:
        case KEY_CATAB:
        case 9:
          m_lines[m_currentY].insert( m_currentX, 2, ' ' );
          m_currentX += 2;
          break;

        default:
          if ( ch >= 32 && ch <= 127 )
          {
            m_lines[m_currentY].insert( m_currentX, 1, ( int )ch );
            ++m_currentX;
          }

          break;
      }

      break;
  }
}

void TEG::Print()
{
  for ( size_t i {}; i < LINES - 1; ++i )
  {
    if ( i >= m_lines.size() )
    {
      move( i, 0 );
      clrtoeol();
    }
    else
      mvprintw( i, 0, m_lines[i + m_shiftY].c_str() );

    clrtoeol();
  }

  if ( m_currentY != ( LINES - 1 + m_shiftY ) )
    move( m_currentY - m_shiftY, m_currentX );
}

void TEG::Remove( int number )
{
  m_lines.erase( m_lines.begin() + number );
}

std::string TEG::Tabs( std::string& line )
{
  std::size_t tab = line.find( '\t' );
  return tab == line.npos ? line : Tabs( line.replace( tab, 1, "  " ) );
}

void TEG::Insert( std::string line, int number )
{
  line = Tabs( line );
  m_lines.insert( m_lines.begin() + number, line );
}

void TEG::Append( std::string& line )
{
  line = Tabs( line );
  m_lines.push_back( line );
}

void TEG::Up()
{
  if ( m_currentY > 0 )
    --m_currentY;

  if ( m_currentX >= m_lines[m_currentY].length() )
    m_currentX = m_lines[m_currentY].length();

  if ( m_currentY < m_shiftY )
    --m_shiftY;

  if ( m_currentY > m_shiftY )
    move( m_currentY - m_shiftY, m_currentX );
}

void TEG::Left()
{
  if ( m_currentX > 0 )
  {
    --m_currentX;
    move( m_currentY - m_shiftY, m_currentX );
  }
}

void TEG::Right()
{
  if ( ( int )m_currentX <= COLS && m_currentX <= m_lines[m_currentY].length() - 1 && !m_lines[m_currentY].empty() )
  {
    ++m_currentX;
    move( m_currentY - m_shiftY, m_currentX );
  }
}

void TEG::Down()
{
  if ( m_currentY < m_lines.size() - 1 )
    ++m_currentY;

  if ( m_currentX >= m_lines[m_currentY].length() )
    m_currentX = m_lines[m_currentY].length();

  if ( m_currentY > ( LINES - 2 + m_shiftY ) )
  {
    ++m_shiftY;
    return;
  }

  if ( m_currentY != ( LINES - 1 + m_shiftY ) )
    move( m_currentY - m_shiftY, m_currentX );
}

void TEG::Open()
{
  std::fstream inputStream;
  inputStream.open( m_fileName, std::ios_base::in | std::ios_base::app );

  if ( !inputStream.is_open() )
    throw std::runtime_error( "Cannot open file. Permission denied." );

  while ( !inputStream.eof() )
  {
    std::string buffer;
    std::getline( inputStream, buffer );
    Append( buffer );
  }

  inputStream.close();
}

void TEG::Save()
{
  std::fstream outputStream;
  outputStream.open( m_fileName, std::ios_base::out );

  if ( !outputStream.is_open() )
    throw std::logic_error( "File was not open" );

  for ( size_t i {}; i < m_lines.size(); ++i )
    outputStream << m_lines[i] << std::endl;

  outputStream.close();
}
