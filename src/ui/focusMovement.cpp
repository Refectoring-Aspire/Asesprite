/***********************************************************************
                            Focus Movement
 ***********************************************************************/

// to-do rewrite this function, it is based in an old code from the
//      Allegro library GUI code
void Manager::createList(vector<Widget *> list, Widget *it)
{
  if (does_accept_focus(it) && !(child_accept_focus(it, true)))
    list.push_back(it);
}

// Who have the focus
Window Manager::checkFocusHolder()
{
  if (focus_widget)
  {
    return focus_widget->window();
  }
  else if (!this->children().empty())
  {
    return this->getTopWindow();
  }
}

void Manager::checkKeyPressed()
{
  // Depending on the pressed key...
  switch (static_cast<KeyMessage *>(msg)->scancode())
  {

  case kKeyTab:
    // Reverse tab
    if ((msg->modifiers() & (kKeyShiftModifier | kKeyCtrlModifier | kKeyAltModifier)) != 0)
    {
      focus = list[count - 1];
    }
    // Normal tab
    else if (count > 1)
    {
      focus = list[1];
    }
    ret = true;
    break;

  // Arrow keys *****
  case kKeyLeft:
    if (!cmp)
      cmp = cmp_left;
  case kKeyRight:
    if (!cmp)
      cmp = cmp_right;
  case kKeyUp:
    if (!cmp)
      cmp = cmp_up;
  case kKeyDown:
    if (!cmp)
      cmp = cmp_down;
  }
}
bool Manager::processFocusMovementMessage(Message *msg)
{
  int (*cmp)(Widget *, int, int) = NULL;
  Widget *focus = NULL;
  Widget *it;
  bool ret = false;
  Window *window = NULL;
  int c, count;

  //**// Who have the focus
  window = checkFocusHolder();

  if (!window)
    return false;

  // How many children want the focus in this window?
  count = count_widgets_accept_focus(window);

  // One at least
  if (count > 0)
  {
    std::vector<Widget *> list(count);

    c = 0;

    //**changed**

    // Create a list of possible candidates to receive the focus
    // for (it=focus_widget; it; it=next_widget(it)) {
    //   if (does_accept_focus(it) && !(child_accept_focus(it, true)))
    //     list[c++] = it;
    // }
    // for (it=window; it != focus_widget; it=next_widget(it)) {
    //   if (does_accept_focus(it) && !(child_accept_focus(it, true)))
    //     list[c++] = it;
    // }

    for (it = focus_widget; it; it = next_widget(it))
    {
      createList(list, it);
    }
    for (it = window; it != focus_widget; it = next_widget(it))
    {
      createList(list, it);
    }

    //** // Depending on the pressed key...
    checkKeyPressed();

    // More than one widget
    if (count > 1)
    {
      // Position where the focus come
      gfx::Point pt = (focus_widget ? focus_widget->bounds().center() : window->bounds().center());

      c = (focus_widget ? 1 : 0);

      //**change**
      // Rearrange the list
      // for (int i = c; i < count - 1; ++i)
      // {
      //   for (int j = i + 1; j < count; ++j)
      //   {
      //     // Sort the list in ascending order
      //     if ((*cmp)(list[i], pt.x, pt.y) > (*cmp)(list[j], pt.x, pt.y))
      //       std::swap(list[i], list[j]);
      //   }
      // }

      int n = sizeof(list) / sizeof(list[0]);
      sort(list, list + n);

#ifdef REPORT_FOCUS_MOVEMENT
      // Print list of widgets
      for (int i = c; i < count - 1; ++i)
      {
        TRACE("list[%d] = %d (%s)\n",
              i, (*cmp)(list[i], pt.x, pt.y),
              typeid(*list[i]).name());
      }
#endif

      // Check if the new widget to put the focus is not in the wrong way.
      if ((*cmp)(list[c], pt.x, pt.y) < std::numeric_limits<int>::max())
        focus = list[c];
    }
    s
        // If only there are one widget, put the focus in this
        else focus = list[0];

    ret = true;
    break;
  }

  if ((focus) && (focus != focus_widget))
    setFocus(focus);

  return ret;
}

static int count_widgets_accept_focus(Widget *widget)
{
  int count = 0;

  for (auto child : widget->children())
    count += count_widgets_accept_focus(child);

  if ((count == 0) && (does_accept_focus(widget)))
    count++;

  return count;
}

static bool child_accept_focus(Widget *widget, bool first)
{
  for (auto child : widget->children())
    if (child_accept_focus(child, false))
      return true;

  return (first ? false : does_accept_focus(widget));
}

static Widget *next_widget(Widget *widget)
{
  if (!widget->children().empty())
    return UI_FIRST_WIDGET(widget->children());

  while (widget->parent() &&
         widget->parent()->type() != kManagerWidget)
  {
    WidgetsList::const_iterator begin = widget->parent()->children().begin();
    WidgetsList::const_iterator end = widget->parent()->children().end();
    WidgetsList::const_iterator it = std::find(begin, end, widget);

    ASSERT(it != end);

    if ((it + 1) != end)
      return *(it + 1);
    else
      widget = widget->parent();
  }

  return NULL;
}

static int cmp_left(Widget *widget, int x, int y)
{
  int z = x - (widget->bounds().x + widget->bounds().w / 2);
  if (z <= 0)
    return std::numeric_limits<int>::max();
  return z + ABS((widget->bounds().y + widget->bounds().h / 2) - y) * 8;
}

static int cmp_right(Widget *widget, int x, int y)
{
  int z = (widget->bounds().x + widget->bounds().w / 2) - x;
  if (z <= 0)
    return std::numeric_limits<int>::max();
  return z + ABS((widget->bounds().y + widget->bounds().h / 2) - y) * 8;
}

static int cmp_up(Widget *widget, int x, int y)
{
  int z = y - (widget->bounds().y + widget->bounds().h / 2);
  if (z <= 0)
    return std::numeric_limits<int>::max();
  return z + ABS((widget->bounds().x + widget->bounds().w / 2) - x) * 8;
}

static int cmp_down(Widget *widget, int x, int y)
{
  int z = (widget->bounds().y + widget->bounds().h / 2) - y;
  if (z <= 0)
    return std::numeric_limits<int>::max();
  return z + ABS((widget->bounds().x + widget->bounds().w / 2) - x) * 8;
}
