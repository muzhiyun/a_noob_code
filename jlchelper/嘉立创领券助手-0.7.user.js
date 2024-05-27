// ==UserScript==
// @name         嘉立创领券助手
// @namespace    http://tampermonkey.net/
// @version      0.7
// @description  Check coupon validity and disable if conditions are not met, and auto-click valid coupons
// @author       Your name
// @match        https://www.szlcsc.com/huodong.html*
// @grant        none
// ==/UserScript==

(function() {
    'use strict';

    // 默认阈值
    let threshold = 5;

    // 是否置灰新人专享开关
    let disableNewUserCoupons = true;

    // 是否删除不符合条件的元素开关
    let deleteInvalidElements = true;

    // 获取所有coupon-item元素
    let couponItems = document.querySelectorAll('.coupon-item');

    // 遍历每个coupon-item元素
    couponItems.forEach(item => {

        let condition = parseInt(item.querySelector('.condition').textContent.trim().replace('满', '').replace('可用', ''));
        let moneyElement = item.querySelector('.money');
        let money = condition; // 默认设置为condition，以防止空指针异常
        if (moneyElement) {
            let moneyText = moneyElement.textContent.trim().match(/\d+/);
            if (moneyText) {
                money = parseInt(moneyText[0]);
            }
        }
        let couponName = item.querySelector('.coupon-item-name h3').textContent.trim();

        // 判断差值是否在阈值以内
        if (Math.abs(money - condition) > threshold) {
            if (deleteInvalidElements) {
                item.remove(); // 删除不符合条件的元素
            } else {
                disableElement(item);
            }
        }

        // 判断是否置灰新人专享
        if (disableNewUserCoupons && couponName.includes('<新人专享>')) {
            if (deleteInvalidElements) {
                item.remove(); // 删除不符合条件的元素
            } else {
                disableElement(item);
            }
        }
    });



    // 监听页面加载完成事件
    document.addEventListener('DOMContentLoaded', function() {
        // 添加一个按钮到页面
        console.debug("muzhi添加按钮DOMContentLoaded");
        let autoClickButton = document.createElement('button');
        autoClickButton.textContent = '自动领取符合条件的券';
        autoClickButton.style.marginTop = '20px';
        autoClickButton.addEventListener('click', function() {
            autoClickCoupons();
        });
        document.body.appendChild(autoClickButton);

        // 将页面回滚至最上方
        //window.scrollTo(0, 0);
    });
    console.debug("muzhi添加按钮开始");
        // 延迟执行添加按钮的代码
    setTimeout(function() {
        console.debug("muzhi开始添加按钮");
        // 添加一个按钮到页面
        let autoClickButton = document.createElement('button');
        autoClickButton.textContent = '22自动领取符合条件的券';
        autoClickButton.style.marginTop = '20px';
        console.debug("muzhi 定时添加按钮");
        autoClickButton.addEventListener('click', function() {
            autoClickCoupons();
        });
        document.body.appendChild(autoClickButton);
    }, 2000); // 延迟2秒执行
console.debug("muzhi添加按钮结束");
    // 禁用元素的函数
    function disableElement(item) {
        let btnText = item.querySelector('.coupon-item-btn-text');
        let btn = item.querySelector('.coupon-item-btn');
        btnText.style.color = 'gray';
        btnText.textContent = '不可点击'; // 修改为不可点击状态
        btn.style.pointerEvents = 'none'; // 禁用点击
        btn.style.background = 'gray';
        btn.style.color = 'gray';
    }

    // 自动点击符合条件的券
    function autoClickCoupons() {
        let validCoupons = document.querySelectorAll('.coupon-item:not([style*="display: none;"])');
        validCoupons.forEach(coupon => {
            let btn = coupon.querySelector('.coupon-item-btn');
            if (btn) {
                btn.click();
            }
        });
    }
})();
