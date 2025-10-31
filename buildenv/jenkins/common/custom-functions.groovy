/******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 2020, 2026 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 ******************************************************************************/

def set_extra_options() {
    set_healthcenter()
}

def set_healthcenter() {
    def hcAgentDownloadUrl = buildspec.getScalarField("healthcenter.agent", SDK_VERSION)

    if (hcAgentDownloadUrl) {
        def hcAgentArchName = hcAgentDownloadUrl.tokenize('/').getAt(-1)

        curl_artifactory(hcAgentDownloadUrl)

        if (fileExists("${WORKSPACE}/${hcAgentArchName}")) {
            // add --with-healthcenter configure option
            EXTRA_CONFIGURE_OPTIONS += " --with-healthcenter=${WORKSPACE}/${hcAgentArchName}"
        }
    }
}

def curl_artifactory(downloadURL, outputFile = null) {
    // env.ARTIFACTORY_SERVER should be already set in build.groovy
    // see set_basic_artifactory_config() in variables-functions.groovy
    def server = Artifactory.server env.ARTIFACTORY_SERVER
    def artifactoryCredentialsID = server.getCredentialsId()

    def outputOption = '-O'
    if (outputFile) {
        outputOption = "-o ${outputFile}"
    }
    withCredentials([usernamePassword(credentialsId: "${artifactoryCredentialsID}", usernameVariable: "ARTIFACTORY_USERNAME", passwordVariable: "ARTIFACTORY_TOKEN")]) {
        sh "_ENCODE_FILE_NEW=UNTAGGED curl -sSkL -u \$ARTIFACTORY_USERNAME:\$ARTIFACTORY_TOKEN ${downloadURL} ${outputOption}"
    }
}

return this
