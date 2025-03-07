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

                implementation(libs.testsuite.net)
                implementation(libs.testsuite.net.openjdk)
                implementation(libs.api.net)

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

                            // do not pass standalone tests, only group tests
                            excludeTestsMatching("com.microej.net.test.standalone*")
                            // Exclude tests for Windows
                            excludeTestsMatching("com.microej.net.test.group.TestNetAdditionalWindows")
                            // Exclude tests that use unspported socket options
                            excludeTestsMatching("com.microej.net.test.group.TestDatagramSocketAdditional")
                            excludeTestsMatching("com.microej.net.test.group.TestDatagramSocketSetBufferSize")
                            excludeTestsMatching("com.microej.net.test.group.TestSocketOptionsSendRecvBuffer")
                            excludeTestsMatching("com.microej.net.test.group.TestSocketOptionsSendRecvBuffer2")
                            excludeTestsMatching("com.microej.net.test.group.TestSocketOptionsOOBInline")
                            // Exclude IPV6 tests
                            excludeTestsMatching("com.microej.net.test.group.dual.TestDualStackSocket")
                            excludeTestsMatching("com.microej.net.test.group.dual.TestDualStackDatagram")
                            excludeTestsMatching("com.microej.net.test.group.dual.TestDualStackServerSocket")
                            // some openjdk tests require a localhost network interface, add this pattern if this is not the case
                            excludeTestsMatching("openjdk*")
                            // do not pass standalone tests, only group tests
                            excludeTestsMatching("com.microej.net.test.standalone*")
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