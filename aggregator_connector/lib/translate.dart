


const _keyTranslation = <String, String>{
  'name': 'Name',
  'author': 'Author',
  'description': 'Description',
  'model': 'Model',
  'voltage': 'Voltage',
  'trigger_threshold': 'Trigger threshold',
  'trigger_edge': 'Trigger edge',
  'samples_before': 'Samples before',
  'samples_after': 'Samples after',
  'sample_freq': 'Sampling frequency',
  'mode': 'Mode',
};

String tr(String text) {
  return _keyTranslation[text] ?? text;
}

