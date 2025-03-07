plugins {
    id("com.microej.gradle.application") version libs.versions.microej.sdk
}

microej {
    applicationEntryPoint = "com.nxp.example.helloworld.Main"
    architectureUsage = System.getProperty("com.microej.architecture.usage") ?: "eval" // or "prod"
    skippedCheckers = "changelog,readme,license,nullanalysis"
    produceExecutableDuringBuild()
}

dependencies {
    implementation(libs.api.edc)
    implementation(libs.api.device)

    implementation(libs.library.logging)

    microejVee(project(":vee-port"))
}
