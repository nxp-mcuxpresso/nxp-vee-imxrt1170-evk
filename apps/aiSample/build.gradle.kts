plugins {
    id("com.microej.gradle.application") version libs.versions.microej.sdk
}

microej {
    applicationEntryPoint = "com.nxp.example.aisample.AiMain"
    architectureUsage = System.getProperty("com.microej.architecture.usage") ?: "eval" // or "prod"
    skippedCheckers = "changelog,readme,license,nullanalysis"
    produceExecutableDuringBuild()
}

dependencies {
    implementation(libs.api.ai)
    implementation(libs.api.drawing)

    implementation(libs.library.widget)
    implementation(libs.library.logging)

    microejVee(project(":vee-port"))
}
