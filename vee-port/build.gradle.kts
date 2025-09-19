plugins {
    id("com.microej.gradle.veeport") version libs.versions.microej.sdk
}

microej {
    skippedCheckers = "changelog,readme,license"
}

dependencies {
    microejArchitecture(libs.architecture)

    // Only 3 packs are installed by default: UI, FS and NET.
    // Comment/uncomment the packs depending on your needs.
    // If the UI pack is removed, do not forget to remove the related
    // dependencies and configuration in the Front Panel project.
    microejPack(libs.pack.ui.architecture)
    microejPack(libs.pack.event)
    microejPack(libs.pack.fs)
    microejPack(libs.pack.gpio)
    microejPack(libs.pack.gpio.properties)
    microejPack(libs.pack.net)
    microejPack(libs.pack.device)
    microejPack(libs.pack.ecom.wifi)
    microejPack(libs.pack.vg)
    microejPack(libs.api.microai)

    microejFrontPanel(project(":vee-port:front-panel"))

    microejMock(project(":vee-port:mock"))
    microejMock(libs.jenkins)

    microejTool(project(":vee-port:image-generator"))
}
