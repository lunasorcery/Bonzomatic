namespace LIFX
{
  bool Open(RENDERER_SETTINGS const& renderSettings);
  void UpdateColor();
  void Poll();
  bool Close();
};