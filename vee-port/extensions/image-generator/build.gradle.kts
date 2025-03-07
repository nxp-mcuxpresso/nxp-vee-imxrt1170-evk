plugins {
    id("com.microej.gradle.j2se-library")
}

microej {
    skippedCheckers = "changelog,readme,license,nullanalysis"
}

dependencies {
    implementation(libs.pack.ui) {
        artifact {
            name = "imageGenerator"
            extension = "jar"
        }
    }
}

// Image Generator jar name must start with "imageGenerator" to be correctly loaded by the VEE Port
tasks.jar {
    archiveBaseName = "imageGenerator-mimxrt1170"
}