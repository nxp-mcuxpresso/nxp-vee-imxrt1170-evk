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
                implementation(libs.junit)
                implementation(libs.junit.platform)

                implementation(libs.api.edc)
                implementation(libs.api.bon)
                implementation(libs.api.microui)
                implementation(libs.api.drawing)
                implementation(libs.testsuite.ui)
            }

            targets {
                all {
                    testTask.configure {
                        filter {
                            excludeTestsMatching("*AllTestClasses")
                            excludeTestsMatching("*SingleTest*")
                            // do not embed inner classes as test classes
                            excludeTestsMatching("*$*")

                            // Test too long for the evaluation license limits.
                            excludeTestsMatching("*Screenshot*")
                            // Test not adapted to i.mxrt1170 screens (pass with a scale equal to 10).
                            excludeTestsMatching("*Scale*")
                            // uncomment this line to fully exclude the GPU tests (useless when there is no GPU on the target)
                            //excludeTestsMatching("com.microej.microui.test.gpu*")
                        }
                    }
                }
            }
        }
    }
}