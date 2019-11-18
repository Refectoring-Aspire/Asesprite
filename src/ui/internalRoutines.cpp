/**********************************************************************
                        Internal routines
 **********************************************************************/

// static
Widget *Manager::findLowestCommonAncestor(Widget *a, Widget *b)
{
  if (!a || !b)
    return nullptr;

  Widget *u = a;
  Widget *v = b;
  int aDepth = 0;
  int bDepth = 0;
  while (u)
  {
    ++aDepth;
    u = u->parent();
  }
  while (v)
  {
    ++bDepth;
    v = v->parent();
  }

  while (aDepth > bDepth)
  {
    --aDepth;
    a = a->parent();
  }
  while (bDepth > aDepth)
  {
    --bDepth;
    b = b->parent();
  }

  while (a && b)
  {
    if (a == b)
      break;
    a = a->parent();
    b = b->parent();
  }
  return a;
}

// static
bool Manager::someParentIsFocusStop(Widget *widget)
{
  if (widget->isFocusStop())
    return true;

  if (widget->parent())
    return someParentIsFocusStop(widget->parent());
  else
    return false;
}

// static
Widget *Manager::findMagneticWidget(Widget *widget)
{
  Widget *found;

  for (auto child : widget->children())
  {
    found = findMagneticWidget(child);
    if (found)
      return found;
  }

  if (widget->isFocusMagnet())
    return widget;
  else
    return NULL;
}

// static
Message *Manager::newMouseMessage(
    MessageType type,
    Widget *widget,
    const gfx::Point &mousePos,
    PointerType pointerType,
    MouseButtons buttons,
    KeyModifiers modifiers,
    const gfx::Point &wheelDelta,
    bool preciseWheel)
{
#ifdef __APPLE__
  // Convert Ctrl+left click -> right-click
  if (widget &&
      widget->isVisible() &&
      widget->isEnabled() &&
      widget->hasFlags(CTRL_RIGHT_CLICK) &&
      (modifiers & kKeyCtrlModifier) &&
      (buttons == kButtonLeft))
  {
    modifiers = KeyModifiers(int(modifiers) & ~int(kKeyCtrlModifier));
    buttons = kButtonRight;
  }
#endif

  Message *msg = new MouseMessage(
      type, pointerType, buttons, modifiers, mousePos,
      wheelDelta, preciseWheel);

  if (widget)
    msg->setRecipient(widget);

  return msg;
}

// static
void Manager::broadcastKeyMsg(Message *msg)
{
  // Send the message to the widget with capture
  if (capture_widget)
  {
    msg->setRecipient(capture_widget);
  }
  // Send the msg to the focused widget
  else if (focus_widget)
  {
    msg->setRecipient(focus_widget);
  }
  // Finally, send the message to the manager, it'll know what to do
  else
  {
    msg->setRecipient(this);
  }
}
