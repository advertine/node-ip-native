{
  'targets': [
    {
      'target_name': 'nodeip',
      'sources': [
        'src/curl/inet_pton.c',
        'src/curl/inet_ntop.c',
        'src/ipaddress.c',
        'src/nodeip.cc'
      ],
      'include_dirs': [
        "<!(node -e \"require('nan')\")",
        'src/curl'
      ],
      'conditions': [
        ['OS=="win"', {
          'msvs_settings': {
            'VCCLCompilerTool': {
              'ExceptionHandling': 1,
              'AdditionalOptions': [
                '/EHsc' # ExceptionHandling=1 is not enough
              ]
            }
          },
          'libraries': [
            '-lWs2_32.lib'
          ]
        }]
      ]
    }
  ]
}
