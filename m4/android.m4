AC_ARG_VAR([ANDROID_HOME],[Path to the installation directory of the Android SDK])
AC_ARG_VAR([NDK_HOME],[Path to the installation directory of the Android NDK])

# _android_path_found_notfound(variable, program, path, notfound, found)
#
# search for a program in path. If found, 'variable' is set to the absolute path,
# then executes 'found', otherwise, 'variable' is set to '' and 'notfound' is run
#
AC_DEFUN([_android_path_found_notfound],
	[AC_PATH_PROG([$1],[$2],[],[$3])
	 AS_IF([test x"$1" = x""],
	 	[AC_SUBST([$1],[])
		${5-:}],
		[AC_SUBST([$1])
		${4-:}])])


# ANDROID_PATH_PROG_NDK_BUILD(found, not-found)
# -----------------------------------
#
# Sets the variable NDK_BUILD to the absolute path to the `ndk-build` program.
# Executes found if the path is found, not-found otherwise
#
AC_DEFUN([ANDROID_PATH_PROG_NDK_BUILD],
	[_android_path_found_notfound([NDK_BUILD],[ndk-build],[$NDK_HOME:$PATH],[$2],[${1-:}])]
)

# ANDROID_PATH_PROG_ANDROID(found, not-found)
# ---------------------------------
#
# Search for the program `android`, and sets the variable ANDROID to
# the absolute path if found.
# 
AC_DEFUN([ANDROID_PATH_PROG_ANDROID],
	 [_android_path_found_notfound([ANDROID],[android],[$ANDROID_HOME:$PATH],[$2],[${1-:}])]
)

# ANDROID_PATH_PROG_DX(found, not-found)
# ____________________________
#
# Search for the program `dx`, and sets the variable DX to the absolute path if found.
# Executes found if the path is found, not-found otherwise
#
AC_DEFUN([ANDROID_PATH_PROG_DX],
	 [_android_path_found_notfound([DX],[dx],[$ANDROID_HOME:$PATH],[$2],[${1-:}])]
)

# ANDROID_PATH_PROG_KEYTOOL(found, not-found)
# ____________________________
# Search for the program `keytool`, and sets the variable KEYTOOL to the absolute path if found.
# Executes found if the path is found, not-found otherwise
# 
AC_DEFUN([ANDROID_PATH_PROG_KEYTOOL],
         [_android_path_found_notfound([KEYTOOL],[keytool],[$ANDROID_HOME:$PATH],[$2],[${1-:}])]
)


# ANDROID_PATH_PROG_AAPT(found, not-found)
# ____________________________
# Search for the program `aapt`, and sets the variable AAPT to the absolute path if found.
# Executes found if the path is found, not-found otherwise
# 
AC_DEFUN([ANDROID_PATH_PROG_AAPT],
         [_android_path_found_notfound([AAPT],[aapt],[$ANDROID_HOME:$PATH],[$2],[${1-:}])]
)

# ANDROID_PATH_PROG_JARSIGNER(found, not-found)
# ____________________________
# Search for the program `jarsigner`, and sets the variable JARSIGNER to the absolute path if found.
# Executes found if the path is found, not-found otherwise
# 
AC_DEFUN([ANDROID_PATH_PROG_JARSIGNER],
         [_android_path_found_notfound([JARSIGNER],[jarsigner],[$ANDROID_HOME:$PATH],[$2],[${1-:}])]
)

# ANDROID_PATH_PROG_ZIPALIGN(found, not-found)
# ____________________________
# Search for the program `zipalign`, and sets the variable ZIPALIGN to the absolute path if found.
# Executes found if the path is found, not-found otherwise
# 
AC_DEFUN([ANDROID_PATH_PROG_ZIPALIGN],
         [_android_path_found_notfound([ZIPALIGN],[zipalign],[$ANDROID_HOME:$PATH],[$2],[${1-:}])]
)

# ANDROID_PATH_PROG_EMULATOR(found, not-found)
# ____________________________
# Search for the program `emulator`, and sets the variable EMULATOR to the absolute path if found.
# Executes found if the path is found, not-found otherwise
# 
AC_DEFUN([ANDROID_PATH_PROG_EMULATOR],
         [_android_path_found_notfound([EMULATOR],[emulator],[$ANDROID_HOME:$PATH],[$2],[${1-:}])]
)

# ANDROID_PATH_PROG_EMULATOR(found, not-found)
# ____________________________
# Search for the program `emulator`, and sets the variable EMULATOR to the absolute path if found.
# Executes found if the path is found, not-found otherwise
# 
AC_DEFUN([ANDROID_PATH_PROG_EMULATOR],
         [_android_path_found_notfound([EMULATOR],[emulator],[$ANDROID_HOME:$PATH],[$2],[${1-:}])]
)

# ANDROID_PATH_PROG_ADB(found, not-found)
# ____________________________
# Search for the program `adb`, and sets the variable ADB to the absolute path if found.
# Executes found if the path is found, not-found otherwise
# 
AC_DEFUN([ANDROID_PATH_PROG_ADB],
         [_android_path_found_notfound([ADB],[adb],[$ANDROID_HOME:$PATH],[$2],[${1-:}])]
)

