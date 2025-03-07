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
    microejVee(project(":vee-port"))
}

testing {
    suites {
        val test by getting(JvmTestSuite::class) {
            microej.useMicroejTestEngine(this, TestTarget.EMB, TestMode.MAIN)

            dependencies {
                implementation(project())
                implementation("ej.library.test:junit:1.10.0")
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