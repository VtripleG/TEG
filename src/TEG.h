#pragma once
#include <string>
#include <vector>

class TEG
{
public:

  TEG( const std::string& fileName = std::string( "NoName.txt" ) );

  ~TEG();

  void Start();

  void Update();

  void StatusLine();

  void Input( );

  void Print();

  void Remove( int );

  std::string Tabs( std::string& );

  void Insert( std::string, int );

  void Append( std::string& );

  void Up();

  void Left();

  void Right();

  void Down();

  void Open();

  void Save();

private:
  std::string m_fileName;
  std::vector<std::string> m_lines;
  int m_currentX;
  int m_currentY;
  int m_shiftY;
  char m_mode;
  std::string m_status;
};
