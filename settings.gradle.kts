rootProject.name = "nxpvee-mimxrt1170-evk"
include("vee-port", "vee-port:front-panel", "vee-port:mock", "vee-port:image-generator")
include("apps:aiSample", "apps:animatedMascot", "apps:simpleGFX", "apps:HelloWorld")

project(":vee-port:front-panel").projectDir = file("vee-port/extensions/front-panel")
project(":vee-port:mock").projectDir = file("vee-port/mock")
project(":vee-port:image-generator").projectDir = file("vee-port/extensions/image-generator")

include("vee-port:validation:ai")
include("vee-port:validation:core")
//include("vee-port:validation:ecom-wifi")
include("vee-port:validation:event-queue")
include("vee-port:validation:fs")
include("vee-port:validation:gpio")
include("vee-port:validation:net")
include("vee-port:validation:security")
include("vee-port:validation:ssl")
include("vee-port:validation:ui")
include("vee-port:validation:vg")
