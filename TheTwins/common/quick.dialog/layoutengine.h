#pragma once

namespace quickdialog
{
  struct column
  {
    ctrlvector controls;
    unsigned int width, height;
    RECT margins;
    column()
    { margins.left = margins.top = margins.right = margins.bottom = 0; }
  };

  typedef std::vector<column> section;

  class layout_engine
  {
    private:
      SIZE layoutsections(abstractwindow &wnd, std::vector<section> &sections, const RECT &padding, const unsigned int minwidth)
      {
        int totalwidth = 0;
        SIZE empty = { 0, 0 };
        std::vector<int> widths(sections.size());
        for (unsigned int i = 0; i < sections.size(); ++i)
        { 
          widths[i] = 0;
          for(unsigned int j = 0; j < sections[i].size(); ++j)
          {
            column &col = sections[i][j];
            ctrlvector::iterator end = sections[i][j].controls.end();
            col.width = 0;
            for(ctrlvector::iterator it = sections[i][j].controls.begin(); it < end; ++it)
            {
              SIZE sz = it->get()->getvisible() ? it->get()->requestsize() : empty;
              const RECT &margins = it->get()->getmargins();
              col.width = max(static_cast<unsigned int>(sz.cx), col.width);
              col.margins.left = max(col.margins.left, margins.left);
              col.margins.right = max(col.margins.right, margins.right);
            }
            widths[i] += col.width + col.margins.right + col.margins.left;
            if (col.controls.size())
            {
              col.margins.top = col.controls.front()->getmargins().top;
              col.margins.bottom = col.controls.back()->getmargins().bottom;
            }
          }
          totalwidth = max(widths[i], totalwidth);
        }

        totalwidth = max(totalwidth, static_cast<int>(minwidth) - (padding.left + padding.right));

        //stretch columns to fit.
        for (unsigned int i = 0; i < sections.size(); ++i)
        {
          if (widths[i] < totalwidth)
          {
            int remaining = totalwidth;
            for(unsigned int j = 0; j < sections[i].size(); ++j)
            {
              sections[i][j].width = (sections[i][j].width * totalwidth / widths[i]);
              remaining -= sections[i][j].width;
            }
            sections[i].back().width += remaining; // ensures correct right alignment (fixes for divisional rounding)
          }
        }

        int y = (sections.empty() || sections.front().empty()) ? 0 : (padding.top + sections.front().front().margins.top);
        //move controls
        for (unsigned int i = 0; i < sections.size(); ++i)
        { 
          int x = padding.left, sectiony = y;
          for(unsigned int j = 0; j < sections[i].size(); ++j)
          {
            x += sections[i][j].margins.left;
            ctrlvector::iterator end = sections[i][j].controls.end();
            int coly = y;
            for(ctrlvector::iterator it = sections[i][j].controls.begin(); it < end; ++it)
            {
              SIZE sz = it->get()->getvisible() ? it->get()->requestsize() : empty;
              coly += it->get()->getmargins().top;
              int x2 = x;
              int w = sz.cx;
              switch (it->get()->getalignment())
              {
                case alStretched:
                  w = sections[i][j].width;
                  break;
                case alLeft:
                  break;
                case alCentre:
                  x2 = x + ((sections[i][j].width - sz.cx) / 2);
                  break;
                case alRight:
                  x2 = x + sections[i][j].width - sz.cx;
                  break;
              }
              it->get()->addtowindow(wnd, x2, coly, w, sz.cy);
              coly += sz.cy + it->get()->getmargins().bottom;
            }
            x += sections[i][j].width + sections[i][j].margins.right;
            sectiony = max(sectiony, coly);
          }
          y = sectiony;
        }
        y += padding.bottom;
        totalwidth += padding.left + padding.right;
        SIZE s = { totalwidth, y };
        return s;
      }

    public:    
      SIZE operator()(abstractwindow &wnd, std::vector<ctrlptr> &controls, const RECT padding, const unsigned int minwidth)
      {
        std::vector<section> sections;
        sections.push_back(section());
      
        section *columns = &sections.back();
        ctrlvector::iterator it = controls.begin();
        while(it != controls.end())
        {
          columns->push_back(column());
          column &col = columns->back();
          while(it != controls.end() && !(*it)->isBreak())
          { col.controls.push_back(*it); ++it; }
          if (it != controls.end())
          {
            if ((*it)->isSectionBreak())
            {
              sections.push_back(section());
              columns = &sections.back();
              columns->push_back(column());
            }
            it++; 
          }
        }
        return layoutsections(wnd, sections, padding, minwidth);
      }
  };
}