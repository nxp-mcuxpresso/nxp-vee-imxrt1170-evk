/*
 * Kotlin
 *
 * Copyright 2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */
import com.microej.gradle.plugins.TestMode
import com.microej.gradle.plugins.TestTarget

plugins {
    id("com.microej.gradle.testsuite")
}

group = "com.microej.pack.microai"

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
            microej.useMicroejTestEngine(this, TestTarget.EMB, TestMode.JUNIT)

            dependencies {
                implementation(project())
                implementation(libs.junit)
                implementation(libs.junit.platform)
                implementation(libs.api.bon)

                implementation(libs.api.microai)
            }

            targets {
                all {
                    testTask.configure {
                        filter {
                            // List of excluded tests
                        }
                    }
                }
            }
        }
    }
}
