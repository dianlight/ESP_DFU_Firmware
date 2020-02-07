Import("env", "projenv")
import subprocess
import os
import time
try:
    import configparser
except ImportError:
    import ConfigParser as configparser

def before_upload(source, target, env):
    env.Dump()
#    print("SOURCE:{}\n TARGET:{}\n ENV:{}\n".format(map(str,source),map(str,target),env.Dump()))
#    print("SOURCE:{}\n TARGET:{}\n ENV:{}\n".format(map(str,source),map(str,target),env.Dump()))
    source_s = source.pop()
#    target_s = target.pop();
#    print("SOURCE:{}\n TARGET:{}\n ".format(source_s,target_s))
#    print("before_upload ", env.get("PIOENV"), env.get("BUILD_DIR"))
    config = configparser.ConfigParser(interpolation=ExtendedInterpolationPIO(),delimiters=('=', '.'))
    config.read("platformio.ini")
    print(config.sections())
    build_env = "env:{}".format(env.get("PIOENV"))
    print("Current Environment: {}".format(build_env),build_env)
    if config.get(build_env,'upload_protocol',fallback="unk") != "espota":
        print("ATTENTION: DFU Firmware work only for espota protocol!")
        return
    s = " "    
    dfu_poject_dir = config.get(build_env,"custom_dfu_project_dir",fallback=config.get("esp_dfu","custom_dfu_project_dir"))
    dfu_build_flags = config.get(build_env,"custom_dfu_build_flags",fallback=config.get("esp_dfu","custom_dfu_build_flags")).replace('\n',' ')
    if source_s.get_size() < (450 * 1024):
        print("No DFU Firmware needed!")
        return    
    print("Size {} is over 50% of the avaliable space. Use DFU Firmware Flags {}.".format(source_s.get_size(),dfu_build_flags))

    subprocess.check_call("PLATFORMIO_BUILD_FLAGS='{}' pio run -d {} -t upload --upload-port {}".format(dfu_build_flags,dfu_poject_dir,config.get(build_env,'upload_port')),shell=True)
    waiting =True
    wait_time = config.getint(build_env,"custom_dfu_wait_time", fallback= 35)
    ping_host = config.get(build_env,"upload_port")
    print("Wait {} second for ESP Boot on {}".format(wait_time,ping_host))
    time.sleep(wait_time)
#    while waiting:
#        counter =0
#        t = os.system("ping {}".format(ping_host))
#        if t:
#            waiting=False
#            print("DFU ready!")
#        else:
#            counter +=1
#            time.sleep(1)
#            if counter == 1000: # this will prevent an never ending loop, set to the number of tries you think it will require
#                waiting = False 


#def after_upload(source, target, env):
#    print("after_upload")
#    # do some actions

#print("Current build targets", map(str, BUILD_TARGETS))

env.AddPreAction("upload", before_upload)
#env.AddPostAction("upload", after_upload)

#
# Custom actions for specific files/objects
#

#env.AddPreAction("$BUILD_DIR/${PROGNAME}.elf", [callback1, callback2,...])
#env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", callback...)
import re
MAX_INTERPOLATION_DEPTH = 10

class ExtendedInterpolationPIO(configparser.Interpolation):
    """Advanced variant of interpolation, supports the syntax used by
    `platformio'. Enables interpolation between sections."""

    _KEYCRE = re.compile(r"\$\{([^}]+)\}")

    def before_get(self, parser, section, option, value, defaults):
        L = []
        self._interpolate_some(parser, option, L, value, section, defaults, 1)
        return ''.join(L)

    def before_set(self, parser, section, option, value):
        tmp_value = value.replace('$$', '') # escaped dollar signs
        tmp_value = self._KEYCRE.sub('', tmp_value) # valid syntax
        if '$' in tmp_value:
            raise ValueError("invalid interpolation syntax in %r at "
                             "position %d" % (value, tmp_value.find('$')))
        return value

    def _interpolate_some(self, parser, option, accum, rest, section, map,
                          depth):
        rawval = parser.get(section, option, raw=True, fallback=rest)
        if depth > MAX_INTERPOLATION_DEPTH:
            raise InterpolationDepthError(option, section, rawval)
        while rest:
            p = rest.find("$")
            if p < 0:
                accum.append(rest)
                return
            if p > 0:
                accum.append(rest[:p])
                rest = rest[p:]
            # p is no longer used
            c = rest[1:2]
            if c == "$":
                accum.append("$")
                rest = rest[2:]
            elif c == "{":
                m = self._KEYCRE.match(rest)
                if m is None:
                    raise InterpolationSyntaxError(option, section,
                        "bad interpolation variable reference %r" % rest)
                path = m.group(1).split('.')
                rest = rest[m.end():]
                sect = section
                opt = option
                try:
                    if len(path) == 1:
                        opt = parser.optionxform(path[0])
                        v = map[opt]
                    elif len(path) == 2:
                        sect = path[0]
                        opt = parser.optionxform(path[1])
                        v = parser.get(sect, opt, raw=True)
                    else:
                        raise configparser.InterpolationSyntaxError(
                            option, section,
                            "More than one '.' found: %r" % (rest,))
                except (KeyError, configparser.NoSectionError, configparser.NoOptionError):
                    raise configparser.InterpolationMissingOptionError(
                        option, section, rawval, ".".join(path)) from None
                if "$" in v:
                    self._interpolate_some(parser, opt, accum, v, sect,
                                           dict(parser.items(sect, raw=True)),
                                           depth + 1)
                else:
                    accum.append(v)
            else:
                raise configparser.InterpolationSyntaxError(
                    option, section,
                    "'$' must be followed by '$' or '{', "
                    "found: %r" % (rest,))
