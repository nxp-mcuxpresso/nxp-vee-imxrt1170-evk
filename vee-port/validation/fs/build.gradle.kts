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

                implementation(libs.testsuite.fs)
                implementation(libs.api.edc)
                implementation(libs.api.bon)
                implementation(libs.api.fs)
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

                            // Exclude Windows tests.
                            excludeTestsMatching("*_Windows")
                            // Exclude tests not supported by fatfs.
                            excludeTestsMatching("com.microej.fs.tests.scenarios.TestMultipleThreadOnSameFile")
                            excludeTestsMatching("com.microej.fs.tests.scenarios.TestMultipleStreamOnSameFile")
                            // Including this test throws a "ClassNotFoundException: ej.lang.ResourceManager"
                            // because resourcemanager-1.0-api not in the classpath used by JUnit to discover tests classes
                            // because it hab to be removed from the test runtime classpath (see dependencies above). 
                            excludeTestsMatching("TestResources01")
                        }
                    }
                }
            }
        }
    }
}