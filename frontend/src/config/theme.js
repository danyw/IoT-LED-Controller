// #fbfbfb - white
// #ee6723 - peach
// #419bf9 - blue
// #c8cf2d - green
// #554d56 - teflon


export const tokens = (mode) => ({
    ...(mode === 'dark'
      ? {
          white: {
            100: '#fefefe',
            200: '#fdfdfd',
            300: '#fdfdfd',
            400: '#fcfcfc',
            500: '#fbfbfb', // white
            600: '#c9c9c9',
            700: '#979797',
            800: '#646464',
            900: '#323232',
          },
          primary: {
            100: '#dddbdd',
            200: '#bbb8bb',
            300: '#99949a',
            400: '#777178',
            500: '#554d56', // teflon
            600: '#443e45',
            700: '#332e34',
            800: '#221f22',
            900: '#110f11',
          },
          redAccent: {
            100: '#fce1d3',
            200: '#f8c2a7',
            300: '#f5a47b',
            400: '#f1854f',
            500: '#ee6723', // peach
            600: '#be521c',
            700: '#8f3e15',
            800: '#5f290e',
            900: '#301507',
          },
          blueAccent: {
            100: '#d9ebfe',
            200: '#b3d7fd',
            300: '#8dc3fb',
            400: '#67affa',
            500: '#419bf9', // blue
            600: '#347cc7',
            700: '#275d95',
            800: '#1a3e64',
            900: '#0d1f32',
          },
          yellowAccent: {
            100: '#f4f5d5',
            200: '#e9ecab',
            300: '#dee281',
            400: '#d3d957',
            500: '#c8cf2d', // green
            600: '#a0a624',
            700: '#787c1b',
            800: '#505312',
            900: '#282909',
          },
        }
      : {
          white: {
            100: '#323232',
            200: '#646464',
            300: '#979797',
            400: '#c9c9c9',
            500: '#fbfbfb', // white
            600: '#fcfcfc',
            700: '#fdfdfd',
            800: '#fdfdfd',
            900: '#fefefe',
          },
          primary: {
            100: '#110f11',
            200: '#221f22',
            300: '#332e34',
            400: '#443e45',
            500: '#554d56', // teflon
            600: '#777178',
            700: '#99949a',
            800: '#bbb8bb',
            900: '#dddbdd',
          },
          redAccent: {
            100: '#301507',
            200: '#5f290e',
            300: '#8f3e15',
            400: '#be521c',
            500: '#ee6723', // peach
            600: '#f1854f',
            700: '#f5a47b',
            800: '#f8c2a7',
            900: '#fce1d3',
          },
          blueAccent: {
            100: '#0d1f32',
            200: '#1a3e64',
            300: '#275d95',
            400: '#347cc7',
            500: '#419bf9', // blue
            600: '#67affa',
            700: '#8dc3fb',
            800: '#b3d7fd',
            900: '#d9ebfe',
          },
          yellowAccent: {
            100: '#282909',
            200: '#505312',
            300: '#787c1b',
            400: '#a0a624',
            500: '#c8cf2d', // green
            600: '#d3d957',
            700: '#dee281',
            800: '#e9ecab',
            900: '#f4f5d5',
          },
        }),
  });
  
  export const themeSettings = (mode) => {
    const colors = tokens(mode);
    return {
      palette: {
        mode: mode,
        ...(mode === 'dark'
          ? {
              primary: {
                main: colors.primary[500],
              },
              secondary: {
                main: colors.yellowAccent[700],
              },
              neutral: {
                dark: colors.white[700],
                main: colors.white[500],
                light: colors.white[100],
              },
              background: {
                default: colors.primary[600],
              },
            }
          : {
              primary: {
                main: colors.primary[300],
              },
              secondary: {
                main: colors.yellowAccent[600],
              },
              neutral: {
                dark: colors.white[700],
                main: colors.white[500],
                light: colors.white[100],
              },
              background: {
                default: colors.white[500],
              },
            }),
      },
      typography: {
        fontFamily: ['Open Sans', 'sans-serif'].join(','),
        fontSize: 12,
        h1: {
          fontFamily: ['Open Sans', 'sans-serif'].join(','),
          fontSize: 40,
        },
        h2: {
          fontFamily: ['Open Sans', 'sans-serif'].join(','),
          fontSize: 32,
        },
        h3: {
          fontFamily: ['Open Sans', 'sans-serif'].join(','),
          fontSize: 24,
        },
        h4: {
          fontFamily: ['Open Sans', 'sans-serif'].join(','),
          fontSize: 20,
        },
        h5: {
          fontFamily: ['Open Sans', 'sans-serif'].join(','),
          fontSize: 16,
        },
        h6: {
          fontFamily: ['Open Sans', 'sans-serif'].join(','),
          fontSize: 14,
        },
      },
    };
  };