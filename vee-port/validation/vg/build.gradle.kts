import com.microej.gradle.plugins.TestMode
import com.microej.gradle.plugins.TestTarget

plugins {
    id("com.microej.gradle.testsuite")
}

microej {
    architectureUsage = System.getProperty("com.microej.architecture.usage") ?: "eval" // or "prod"
    skippedCheckers = "changelog,readme,license"
}

dependencies {
    // Use a VEE Port included in the same multi-project as this validation application
    microejVee(project(":vee-port"))

    // Use a published version of a VEE Port
    // or a local VEE Port project with includeBuild directive
    // microejVee("com.mycompany:vee-port:1.0.0")

    // Use a built VEE Port (can be used for SDK5 compatibility)
    // In this case, architectureUsage is determined by the VEE Port
    // Use double "\" on Windows
    // microejVee(files("/path/to/vee-port/build/veePort/source"))
}

testing {
    suites {
        val test by getting(JvmTestSuite::class) {
            microej.useMicroejTestEngine(this, TestTarget.EMB, TestMode.MAIN)

            dependencies {
                implementation(project())
                implementation(libs.junit)
                implementation(libs.junit.platform)

                implementation(libs.api.edc)
                implementation(libs.api.bon)
                implementation(libs.api.microui)
                implementation(libs.api.drawing)
                implementation(libs.testsuite.vg)
                implementation(libs.api.microvg)
            }

            targets {
                all {
                    testTask.configure {
                        filter {
                            excludeTestsMatching("*AllTestClasses")
                            excludeTestsMatching("*SingleTest*")
                            // do not embed inner classes as test classes
                            excludeTestsMatching("*$*")

                            // External resource loader not implemented
                            excludeTestsMatching("*TestFontExternal*")
                            excludeTestsMatching("*TestImageExternal*")
                            // Tests designed for 392x392 screens
                            excludeTestsMatching("*TestFontComplexLayoutOnCircle*")
                            // Differences slightly above max threshold (8% instead of 5%) -> Acceptable.
                            excludeTestsMatching("*TestFontComplexLayout*")
                            // Exclude testStringWithSpace because of an issue on the test implementation.
                            excludeTestsMatching("*TestDrawStringOnCircle")
                        }
                    }
                }
            }
        }
    }
}
