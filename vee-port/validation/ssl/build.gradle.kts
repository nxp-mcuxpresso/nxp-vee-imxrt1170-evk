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
                implementation(libs.api.ssl)
                implementation(libs.api.net)
                implementation(libs.testsuite.ssl)
                // Add the following dependency if running the testsuite over Wi-Fi.
                // implementation(libs.net.wifi.testsuite)
            }

            targets {
                all {
                    testTask.configure {
                        filter {
                            includeTestsMatching("Test*")
                            excludeTestsMatching("*AllTestClasses")
                            excludeTestsMatching("*SingleTest*")
                            // do not embed inner classes as test classes
                            excludeTestsMatching("*$*")

                            // Exclude test using TLS v1.0, unsupported
                            excludeTestsMatching("TestKeystoreManipulation")
                            // PKI tests are used to check certificate chain validation policies
                            excludeTestsMatching("*TestHandshakePki*")
                        }

                        doFirst {
                            systemProperties = mapOf(
                                "microej.testsuite.properties.remoteapp.timeout" to "300000"
                            )
                        }
                    }
                }
            }
        }
    }
}